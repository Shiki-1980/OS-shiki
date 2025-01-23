#include <aarch64/mmu.h>
#include <common/rc.h>
#include <common/spinlock.h>
#include <driver/memlayout.h>
#include <kernel/mem.h>
#include <kernel/printk.h>

//页内块指针
typedef struct BlockHeader {
    struct BlockHeader* next;
} BlockHeader;

//页内指针
typedef struct PageHeader {
    //节点还需要一个值来存储页首地址
    struct PageHeader* next;//应该使用单链表
    struct PageHeader* prev;
} PageHeader;

typedef struct Header{
    unsigned short block_size;
    unsigned short block_count;
}Header;

typedef struct Allocator {
    unsigned int block_size;   // 块大小
    BlockHeader* free_list;    // 指向空闲块的链表
    bool head;
} Allocator;


#define NUM_ALLOCATORS  31 // 对应 8, 16, 32, 64, ..., 2048 字节的分配块大小
#define PAGE_SIZE 4096
#define ALIGNMENT 8

static Allocator allocators[NUM_ALLOCATORS];
static SpinLock mem_lock;
static SpinLock page_lock;
static SpinLock free_lock;
static PageHeader* free_pages;//空页
static PageHeader* partial_pages;//已分配但未满的页
static struct page page_cnt[(u64)PHYSTOP / (u64)PAGE_SIZE];
int size[NUM_ALLOCATORS]={8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,200,224,256,312,384,448,512,1016,2040,4080};
int page_counter = 0;
RefCount kalloc_page_cnt;


void init_allocators() {
    // unsigned int block_size = 8;
    for (int i = 0; i < NUM_ALLOCATORS; i++) {
        allocators[i].free_list = NULL;  // 初始化时 free_list 为空
        allocators[i].block_size = size[i];
        allocators[i].head=FALSE;
    }
}

// 根据请求大小找到合适的块分配器索引
int get_allocator_index(unsigned long long size) {
    int index = 0;
    unsigned long long current_size = allocators[0].block_size;  // 起始块大小

    while (current_size < size && index < NUM_ALLOCATORS - 1) {
        //current_size <<= 1;  // 每次乘以2
        index++;
        current_size = allocators[index].block_size;
    }
    if(size>allocators[index].block_size){
        printk("分配异常：需求块大小:%lld，分配器块大小:%d\n",size,allocators[index].block_size);
    }
    return index;
}

void kinit() {
    init_rc(&kalloc_page_cnt);
    init_spinlock(&mem_lock);
    init_spinlock(&page_lock);
    init_spinlock(&free_lock);
    
    // 初始化
    init_allocators() ;
    free_pages = NULL;
    partial_pages = NULL;
    
    // Calculate initial free memory range
    extern char end[];
    char* current = (char*)(((unsigned long)end));
    for (u64 p = P2K(0); p < PAGE_BASE((u64)&end) + PAGE_SIZE; p += PAGE_SIZE) {
        inc_page_cnt((void*)p);
    }
    // 对齐到 PAGE_SIZE
    current = (char*)(((unsigned long)current + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
    while (current + PAGE_SIZE <= (char*)(P2K((PHYSTOP)))) {
        PageHeader* page = (PageHeader*)current;
        page->next = free_pages;
        if (free_pages) {
            free_pages->prev = page;
        }
        free_pages = page;
        current += PAGE_SIZE;
        page_counter++;
    }
}
void* kalloc_page() {
    increment_rc(&kalloc_page_cnt);
    acquire_spinlock(&page_lock); // 获取锁

    if (free_pages == NULL) {
        printk("No free pages available!\n");
        release_spinlock(&mem_lock); // 释放锁
        return NULL;
    }

    // 从 free_pages 链表中移除第一个页面
    PageHeader* page = free_pages;
    free_pages = page->next;

    // 如果 free_pages 非空，更新新的链表头部的前向指针为 NULL
    if (free_pages) {
        free_pages->prev = NULL;
    }
    u64 idx = K2P(page) / (u64)PAGE_SIZE;
    init_rc(&(page_cnt[idx].ref));
    release_spinlock(&page_lock); // 释放锁
    return page; // 返回分配的页面
}


void kfree_page(void* p) {
    acquire_spinlock(&page_lock);
    PageHeader* page = (PageHeader*)p;
    // 插入到 free_pages 头部
    page->next = free_pages;
    if (free_pages) {
        free_pages->prev = page;
    }
    free_pages = page;

    decrement_rc(&kalloc_page_cnt);
    release_spinlock(&page_lock);
}

void* kalloc(unsigned long long size) {
    // 1. 获取合适的块分配器索引
    acquire_spinlock(&mem_lock);  // 获取锁
    int index = get_allocator_index(size);
    Allocator* allocator = &allocators[index];
    

    // 2. 检查是否有可用的块
    if (allocator->free_list != NULL) {
        BlockHeader* block = allocator->free_list;  // 获取第一个空闲块
        int nums = 0;  // 用于计数链表中的块数
        BlockHeader* last_block = block;  // 用于保存链表中的最后一个块
     
        // 遍历 free_list，检查块数
        while (block != NULL) {
            nums++;
            if (nums == 2) {  // 找到第二个节点
                break;
            }
            block = block->next;
        }

        // 3. 如果 nums >= 2，说明有返回的块
        if (nums >= 2) {
            block = allocator->free_list;  // 取第一个节点
            last_block = block->next;  // 取第二个节点
            block->next = last_block->next;  // 将第二个节点摘下
            Header* page = (Header*)((unsigned long)last_block & ~(PAGE_SIZE - 1));
            page->block_count--; 
            
            release_spinlock(&mem_lock);  // 释放锁
            return last_block;  // 返回分配的第二个块
        }
        
        // 4. 如果 nums == 1，按顺序分配新块
        if(allocator->head){
        block = allocator->free_list;
        allocator->free_list = NULL;  // 移除当前唯一的块
        unsigned long long next_block_addr = (unsigned long long)block + allocator->block_size;
        
        Header* page = (Header*)((unsigned long)block & ~(PAGE_SIZE - 1));
        // 检查是否还在页面范围内

        if (page->block_count>1 && (next_block_addr & (PAGE_SIZE - 1)) + allocator->block_size <= PAGE_SIZE) {
            
            BlockHeader* next_block = (BlockHeader*)next_block_addr;

            next_block->next = NULL;
            allocator->free_list = next_block;  // 将新块插入 free_list

        }

        page->block_count--;
        
        release_spinlock(&mem_lock);  // 释放锁
        return block;  // 返回已分配的块
        }
        else{
            block = allocator->free_list;
            allocator->free_list=NULL;
            Header* page = (Header*)((unsigned long)block & ~(PAGE_SIZE - 1));
            page->block_count--;
            release_spinlock(&mem_lock);
            return block;
        }
    }
    // 5. 如果没有空闲块，分配新页面并拆分

    void* new_page = (BlockHeader*)kalloc_page();  // 分配新页面
    if (new_page == NULL) {
        printk("kalloc: Failed to allocate new page.\n");
        release_spinlock(&mem_lock);  // 释放锁
        return NULL;  // 分配失败，返回 NULL
    }

     // 初始化新页面的第一个块
     // 计算页首地址，并初始化 Header
    Header* header = (Header*)new_page;
    header->block_size = allocator->block_size;
    header->block_count = (PAGE_SIZE - sizeof(Header)) / allocator->block_size-1;//已分配出去一个
    // 初始化块，从页首的 sizeof(Header) 开始分配
    unsigned long long first_block_addr = (unsigned long long)new_page + sizeof(Header);
    first_block_addr = (first_block_addr + 7) & ~7ULL;  // 对齐到 8 字节
    BlockHeader* first_block = (BlockHeader*)first_block_addr;
    unsigned long long next_block_addr = first_block_addr + allocator->block_size;

    BlockHeader* next_block = (BlockHeader*)next_block_addr;
    next_block->next = NULL;
    allocator->free_list = next_block;
    allocator->head = TRUE;
    release_spinlock(&mem_lock);
   
    return first_block;  // 返回新页面的第一个块
}


void kfree(void* ptr) {
    if (ptr == NULL) {
        printk("kfree: Attempted to free a NULL pointer.\n");
        return;
    }

    acquire_spinlock(&mem_lock);  // 获取锁
    Header* page = (Header*)((unsigned long)ptr & ~(PAGE_SIZE-1 ));  // 获取页首地址
    BlockHeader* block = (BlockHeader*)ptr;  // 确保指针是 BlockHeader 类型

 
    // 2. 根据块大小找到对应的 Allocator
    if(page->block_size>2040){
        release_spinlock(&mem_lock);
        return;
    }

    int index = get_allocator_index(page->block_size);
    Allocator* allocator = &allocators[index];
    // 3. 将释放的块插入到对应的 free_list 链表中
    if(allocator->free_list){
        block->next = allocator->free_list->next;
        allocator->free_list->next = block;

    // 4. 更新该页面的块计数
        page->block_count++;
        }
    else{
        allocator->head = FALSE;
        block->next = allocator->free_list;
        allocator->free_list= block;
        page->block_count++;
    }


    // 5. 如果页面所有块都被释放，则释放整个页面
    if (page->block_count == (PAGE_SIZE - sizeof(Header)) / page->block_size) {
        //printk("kfree: All blocks in page are free, releasing the page.\n");

        // 从 free_list 中移除该页面的所有块
        BlockHeader* current = allocator->free_list;
        BlockHeader* prev = NULL;

        while (current != NULL) {
            // 检查当前块是否属于目标页面
            if (((unsigned long)current & ~(PAGE_SIZE - 1)) == (unsigned long)page) {
                // 如果当前块是头节点
                if (prev == NULL) {
                    // 更新 free_list 的头节点
                    allocator->free_list = current->next;
                } else {
                    // 连接前一个块和下一个块
                    prev->next = current->next;
                }
                // 继续下一个块
                BlockHeader* to_free = current;
                current = current->next;
                // 清除旧的块
                to_free->next = NULL;
            } else {
                // 如果不是目标页面，继续往下遍历
                prev = current;
                current = current->next;
            }
        }
        

        // 释放页面
        kfree_page(page);
    }
    
    release_spinlock(&mem_lock);  // 释放锁
}


void* get_zero_page() {
    return NULL;

}
u64 left_page_cnt(){
    return page_counter-kalloc_page_cnt.count;
}

void inc_page_cnt(void* ka) {
    increment_rc(&(page_cnt[(u64)K2P(ka) / PAGE_SIZE].ref));
}