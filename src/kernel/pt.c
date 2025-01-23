#include <aarch64/intrinsic.h>
#include <common/string.h>
#include <kernel/mem.h>
#include <kernel/pt.h>
#include <aarch64/mmu.h>
#include <kernel/printk.h>
#include <kernel/paging.h>
PTEntriesPtr get_pte(struct pgdir *pgdir, u64 va, bool alloc)
{
    // TODO:
    // Return a pointer to the PTE (Page Table Entry) for virtual address 'va'
    // If the entry not exists (NEEDN'T BE VALID), allocate it if alloc=true, or return NULL if false.
    // THIS ROUTINUE GETS THE PTE, NOT THE PAGE DESCRIBED BY PTE.
    PTEntriesPtr pgtbl = pgdir->pt;

    // 如果一级页表不存在且允许分配，分配一级页表
    if (!pgtbl && alloc) {
        pgtbl = (PTEntriesPtr)kalloc_page();
        memset(pgtbl, 0, PAGE_SIZE);
        pgdir->pt = pgtbl;
    }

    if (!pgtbl) return NULL;

    // 获取虚拟地址的各级索引
    int idx[] = {VA_PART0(va), VA_PART1(va), VA_PART2(va), VA_PART3(va)};


    // 遍历第1到第3级页表
    for (int level = 0; level < 3; level++) {
        PTEntry *pte = &pgtbl[idx[level]];

        // 如果当前页表项无效，并且允许分配新页表
        if (!(*pte & PTE_VALID)) {
            if (!alloc) return NULL;

            // 分配下一级页表
            PTEntriesPtr new_table = (PTEntriesPtr)kalloc_page();
            memset(new_table, 0, PAGE_SIZE);

            // 将新页表的物理地址存入页表项 (需要将虚拟地址转换为物理地址)
            *pte = (u64)K2P(new_table) | PTE_TABLE;
        }

        // 获取下一级页表的内核虚拟地址 (从物理地址转换为虚拟地址)
        pgtbl = (PTEntriesPtr)P2K((PTE_ADDRESS(*pte)));
    }

    // 返回最终第四级页表项的地址
    return &pgtbl[idx[3]];
}

void init_pgdir(struct pgdir *pgdir)
{
    pgdir->pt = kalloc_page();
    memset(pgdir->pt,0,PAGE_SIZE);
    init_spinlock(&pgdir->lock);
    init_list_node(&pgdir->section_head);
    init_sections(&pgdir->section_head);
}

    // 定义一个递归函数来释放页表
void free_pagetable(PTEntriesPtr pt, int level) {
    // level最大为3，超过这个层次的页表项是指向实际的物理页，不再递归
    if (level == 4) return;

    for (int i = 0; i < N_PTE_PER_TABLE; i++) {
        PTEntry pte = pt[i];

        if (pte & PTE_VALID) {
            // 如果是页表项（指向下一级页表）
            if ((pte & PTE_TABLE) == PTE_TABLE) {
                // 获取下一级页表的地址
                PTEntriesPtr next_pt = (PTEntriesPtr)P2K(PTE_ADDRESS(pte));
                free_pagetable(next_pt, level + 1);  // 递归释放下一级页表

                // 释放当前级别页表项所指向的下一级页表
                if(level<=2)
                    kfree_page((void *)next_pt);
            }
            // 如果是叶子节点（即指向实际物理页），不进行释放
        }
    }
}


void free_pgdir(struct pgdir *pgdir)
{
    if (!pgdir->pt) return;
    free_pagetable(pgdir->pt, 0);
    // 释放顶级页表
    kfree_page((void *)pgdir->pt);
    pgdir->pt = NULL;
}

void attach_pgdir(struct pgdir *pgdir)
{
    extern PTEntries invalid_pt;
    if (pgdir->pt)
        arch_set_ttbr0(K2P(pgdir->pt));
    else
        arch_set_ttbr0(K2P(&invalid_pt));
}

/**
 * Map virtual address 'va' to the physical address represented by kernel
 * address 'ka' in page directory 'pd', 'flags' is the flags for the page
 * table entry.
 */
void vmmap(struct pgdir *pd, u64 va, void *ka, u64 flags)
{
    /* (Final) TODO BEGIN */
    auto pte = get_pte(pd,va,true);
    *pte = K2P(ka)|flags;
    inc_page_cnt(ka);
    return ;
    /* (Final) TODO END */
}

/*
 * Copy len bytes from p to user address va in page table pgdir.
 * Allocate physical pages if required.
 * Useful when pgdir is not the current page table.
 */
int copyout(struct pgdir *pd, void *va, void *p, usize len)
{
    /* (Final) TODO BEGIN */
    void * page;
    PTEntriesPtr pte;
    usize n,offset;
    if((usize) va + len > USERTOP)
        return -1;
    while(len>0){
        offset = (usize) va % PAGE_SIZE;
        
        if((pte = get_pte(pd,(u64)va , true)) == NULL)
            return -1;
        //如果pte有效，则不用另外分配页，可直接获取这页
        if(*pte & PTE_VALID){
            page =(void*)P2K(PTE_ADDRESS(*pte));
        }
        //需要新分配页
        else{
            if((page = kalloc_page())==NULL){
                return -1;
            }
            *pte = K2P(page)|PTE_USER_DATA;//让pte指向物理页
        }
        //写的过程要取最小，否则会跨页！！！
        n = MIN(PAGE_SIZE-offset, len);
        if (p){
            memmove(page + offset, p, n);
            p += n;
        }
        else
            memset(page + offset, 0, n);
        len -= n,va += n;
        }
    return 0;
    /* (Final) TODO END */
}

// #define VA_PART0(va) (((u64)(va) & 0xFF8000000000) >> 39)
// #define VA_PART1(va) (((u64)(va) & 0x7FC0000000) >> 30)
// #define VA_PART2(va) (((u64)(va) & 0x3FE00000) >> 21)
// #define VA_PART3(va) (((u64)(va) & 0x1FF000) >> 12)

struct pgdir* uvmcopy(struct pgdir* pgdir) {
    // 分配一个新的页目录结构体
    struct pgdir* newpgdir = kalloc(sizeof(struct pgdir));
    if (!newpgdir)
        return 0; // 内存分配失败，返回空指针

    // 初始化新的页目录
    init_pgdir(newpgdir);

    // 遍历页目录的每一项
    for (int i = 0; i < N_PTE_PER_TABLE; i++) {
        if (pgdir->pt[i] & PTE_VALID) { // 如果该页表项有效

            // 获取第一级页表的指针
            PTEntriesPtr pgt1 = (PTEntriesPtr)P2K(PTE_ADDRESS(pgdir->pt[i]));
            for (int i1 = 0; i1 < N_PTE_PER_TABLE; i1++) {
                if (pgt1[i1] & PTE_VALID) { // 如果该页表项有效

                    // 获取第二级页表的指针
                    PTEntriesPtr pgt2 = (PTEntriesPtr)P2K(PTE_ADDRESS(pgt1[i1]));
                    for (int i2 = 0; i2 < N_PTE_PER_TABLE; i2++) {
                        if (pgt2[i2] & PTE_VALID) { // 如果该页表项有效

                            // 获取第三级页表的指针
                            PTEntriesPtr pgt3 = (PTEntriesPtr)P2K(PTE_ADDRESS(pgt2[i2]));

                            // 遍历页表的每一项
                            for (int i3 = 0; i3 < N_PTE_PER_TABLE; i3++) {
                                if (pgt3[i3] & PTE_VALID) { // 如果该页表项有效
                                    // 计算虚拟地址va
                                    u64 va = (u64)i << 39 | (u64)i1 << 30 | (u64)i2 << 21 | i3 << 12;
                                    // 获取原页面的物理地址
                                    u64 pa = PTE_ADDRESS(pgt3[i3]);
                                    // 分配一个新的物理页面
                                    void* np = kalloc_page();
                                    // 将原页面的数据复制到新页面
                                    memmove(np, (void*)P2K(pa), PAGE_SIZE);
                                    // 在新的页目录中获取对应的页表项
                                    auto pte = get_pte(newpgdir, va, true);

                                    // 设置新的页表项，指向新分配的页面，并设置用户数据属性
                                    *pte = K2P(np) | PTE_USER_DATA;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return newpgdir; // 返回新的页目录
}
int uvmalloc(struct pgdir * pgdir , u64 oldsz, u64 newsz)
{

  if(newsz < oldsz)
    return oldsz;

  for( u64 addr = ((oldsz +PAGE_SIZE -1)/PAGE_SIZE)*PAGE_SIZE ; addr <newsz ;addr += PAGE_SIZE){
    //先对齐一下
    void *p = kalloc_page();
    ASSERT(p != NULL);
    auto pte = get_pte(pgdir,addr,true) ;
    *pte = K2P(p)|PTE_USER_DATA;
  }
  //就是新分配页？好像没问题
  return newsz;
}

void uvmunmap(struct pgdir * pgdir ,u64 va ,u64 npages ,int do_free){
    u64 a;
    PTEntriesPtr pte;
    if((va % PAGE_SIZE)!= 0){
        printk("uvmunmap: not aligned");
        PANIC();
    }
    
    for(a = va;a< va + npages * PAGE_SIZE; a+= PAGE_SIZE){
        if((pte = get_pte(pgdir, a ,false)) == NULL){
            printk("uvmunmap: get_pte failed");
            PANIC();
        }    
        if((*pte & PTE_VALID)==0){
            printk("uvmunmap: page not present");
            PANIC();
        }
        if(PTE_FLAGS(*pte) == PTE_VALID){
            printk("uvmunmap: not a leaf");
            PANIC();
        }
        if(do_free){
            kfree((void *)*pte);
        }
        *pte = 0;
    }
}

