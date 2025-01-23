#include <aarch64/mmu.h>
#include <common/defines.h>
#include <common/list.h>
#include <common/sem.h>
#include <common/string.h>
#include <fs/block_device.h>
#include <fs/cache.h>
#include <kernel/mem.h>
#include <kernel/paging.h>
#include <kernel/printk.h>
#include <kernel/proc.h>
#include <kernel/pt.h>
#include <kernel/sched.h>

void rest_init();
/*
struct section {
    u64 flags;
    u64 begin;
    u64 end;
    ListNode stnode;

    // The following fields are for the file-backed sections. 

    struct file *fp;
    u64 offset; // Offset in file
    u64 length; // Length of mapped content in file
};
*/
void init_sections(ListNode *section_head) {
    /* (Final) TODO BEGIN */
    struct section* heap = kalloc(sizeof(struct section));
    heap->flags = ST_HEAP;
    heap->begin =0;
    heap->end = 0;
    init_list_node(&(heap->stnode));
    _insert_into_list(section_head,&(heap->stnode));
    /* (Final) TODO END */
}

void free_sections(struct pgdir *pd) {
    /* (Final) TODO BEGIN */
    auto node = pd->section_head.next;
    while(node != &(pd->section_head)) {
        auto sec = container_of(node, struct section,stnode);
        for(u64 i = PAGE_BASE(sec->begin);i< sec->end; i+= PAGE_SIZE){
            auto pte = get_pte(pd,i,false);//得到物理页
            if(pte!= NULL && (*pte & PTE_VALID)) {
                //释放
                u64 va = P2K(PTE_ADDRESS(*pte));
                kfree_page((void *)va);
            }
        }
        node = node->next;//释放下一个节点
        _detach_from_list(&(sec->stnode));//把头结点释放掉
        kfree((void *) sec);
    }
    /* (Final) TODO END */
}

u64 sbrk(i64 size) {
    /**
     * (Final) TODO BEGIN 
     * 
     * Increase the heap size of current process by `size`.
     * If `size` is negative, decrease heap size. `size` must
     * be a multiple of PAGE_SIZE.
     * 
     * Return the previous heap_end.
     */
    Proc* cur = thisproc();
    //先找到是哪一段
    ListNode* node = cur->pgdir.section_head.next;
    auto pd=&cur->pgdir;
    struct section* section = container_of(node,struct section,stnode);
    u64 begin=0;
    while(!(section->flags & ST_HEAP)){
        node = node->next;
        section = container_of(node, struct section, stnode);
        //可能找不到对应的段
        begin = MAX(begin,section->end);
        if(node == &(cur->pgdir.section_head)){
            break;//遍历完了
        }    
    }
    if(node == &(cur->pgdir.section_head)){
        //说明没有heap，则创建一个
        section = (struct section *) kalloc(sizeof(struct section));
        section->flags = ST_HEAP;
        section->begin = begin;
        section->end = section->begin;
        init_list_node(&(section->stnode));
        _insert_into_list(&(cur->pgdir.section_head),&(section->stnode));
    }

    u64 old_end = section->end;
    section->end += size *PAGE_SIZE;
    if(size < 0){
        for(i64 i =0; i<-size ;i++){
            auto pte = get_pte(pd,old_end + i*PAGE_SIZE,false);
            if(pte!= NULL && (*pte & PTE_VALID)){
                //释放
                u64 va = P2K(PTE_ADDRESS(*pte));
                kfree_page((void *)va);
                *pte = NULL;
            }
        }
    }
    attach_pgdir(pd);
    arch_tlbi_vmalle1is();
    return old_end;
    /* (Final) TODO END */
}

int mappages(struct pgdir * pgdir, u64 va, u64 size, u64 pa, int perm)
{
  u64 a, last;
  PTEntriesPtr pte;
  if((va % PAGE_SIZE) != 0){
    printk("mappages: va not aligned\n");
    PANIC();
  }
  if((size % PAGE_SIZE) != 0){
    printk("mappages: size not aligne}d\n");
    PANIC();
  
  }
  if(size == 0){
    printk("mappages: size\n");
    PANIC();
  }
  a = va;
  last = va + size - PAGE_SIZE;
  for(;;){
    if((pte = get_pte(pgdir, a, true)) == 0)
      return -1;
    *pte = pa | perm | PTE_VALID;
    if(a == last)
      break;
    a += PAGE_SIZE;
    pa += PAGE_SIZE;
  }
  return 0;
}

int pgfault_handler(u64 iss) {
    Proc *p = thisproc();
    struct pgdir *pd = &p->pgdir;

    u64 addr =
            arch_get_far(); // Attempting to access this address caused the page fault

    /** 
     * (Final) TODO BEGIN
     * 
     * 1. Find the section struct which contains the faulting address `addr`.
     * 2. Check section flags to determine page fault type.
     * 3. Handle the page fault accordingly.
     * 4. Return to user code or kill the process.
     */
    //printk("elr:%p\n",(void *)p->ucontext->elr);
    //printk("in pgfault:0x%p\n",(void *)addr);
    Vma* v = p->vma;
    while(v!=NULL){
        if(addr>= v-> start && addr < v->end )
            break;
        v = v->next;
    }
    if( v == NULL){
        goto pgfault;
    }

    addr = PAGE_BASE((addr));
    void * mem = kalloc_page();
    if(mem==NULL)
        return -4;//kalloc failed

    memset(mem,0,PAGE_SIZE);

    if(mappages(&p->pgdir, addr, PAGE_SIZE, (u64)mem, v->permission) != 0){
        kfree(mem);
        return -5; // map page failed
    }

    File *f=v->file;
    inodes.lock(f->ip);
    inodes.read(f->ip,(u8*)mem,v->off+addr - v->start, PAGE_SIZE);
    inodes.unlock(f->ip);
    //printk("v->start: %p\n",(void*)v->start);
    return 0;

pgfault:
    struct section *sec = NULL;
    _for_in_list(p,&pd->section_head){
        if (p==&pd->section_head) continue;
        sec=container_of(p,struct section,stnode);
        if (addr>=sec->begin) break;
    }
    auto pte = get_pte(pd,addr,true);
    if(*pte ==NULL){
        //缺页，新分配
        *pte = K2P(kalloc_page())|PTE_USER_DATA;
    }
    else if (*pte & PTE_RO) {
		// copy on write
		void* old_page = (void*)P2K(PTE_ADDRESS(*pte));
		void* new_page =kalloc_page();
		memmove(p, (void *)P2K(PTE_ADDRESS(*pte)), PAGE_SIZE);
		* pte = K2P(new_page) |PTE_USER_DATA;
		kfree_page(old_page);
        }
    else {
        *pte = K2P(kalloc_page())|PTE_USER_DATA;
    }
    attach_pgdir(pd);
    arch_tlbi_vmalle1is();
    return iss;
    /* (Final) TODO END */
}

void copy_sections(ListNode *from_head, ListNode *to_head)
{
    /* (Final) TODO BEGIN */
    ListNode * src = from_head->next;
    ListNode * dst = to_head;
    while(src != from_head){
        auto old_section = container_of(src , struct section , stnode);
        struct section * new_section =kalloc(sizeof(struct section));
        new_section->flags = old_section-> flags;
        new_section->begin = old_section->begin;
        new_section->end = old_section->end;
        new_section->length = old_section->length;
        new_section->offset = old_section->offset;
        if(old_section->fp){
            new_section->fp = file_dup(old_section->fp);
        }
        _insert_into_list(dst,&(new_section->stnode));
        src = src->next;
        dst = dst->next;
    }
    /* (Final) TODO END */
}