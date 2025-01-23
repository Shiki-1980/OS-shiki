#include <kernel/proc.h>
#include <kernel/mem.h>
#include <kernel/sched.h>
#include <aarch64/mmu.h>
#include <common/list.h>
#include <common/string.h>
#include <kernel/printk.h>
#include <kernel/paging.h>
#include <kernel/cpu.h>
#include <kernel/pt.h>

Proc root_proc;
PidNode *pid_recycle_list = NULL;  // PID回收链表的头部指针
Vma vma_list[NVMA];

void kernel_entry();
void proc_entry();

static int pid;
static SpinLock pidlock;
static SpinLock elock;
static SpinLock klock;

extern void writeback(struct vma* v, u64 addr, int n);
//回收pid，加入链表
void recycle_pid(int pid) {
    PidNode *node = kalloc(sizeof(PidNode));
    acquire_spinlock(&pidlock);
    node->pid = pid;
    node->next = pid_recycle_list;
    pid_recycle_list = node;
    release_spinlock(&pidlock);
}

//获取pid
int get_pid() {
    if (pid_recycle_list) {
        // 如果回收链表不为空，从链表中取出一个PID
        acquire_spinlock(&pidlock);
        PidNode *node = pid_recycle_list;
        int recycled_pid = node->pid;
        pid_recycle_list = node->next;
        kfree(node);  // 释放链表节点
        release_spinlock(&pidlock);
        return recycled_pid;
    } else {
        // 如果回收链表为空，正常自增PID
        return ++pid;
    }
}


// init_kproc initializes the kernel process
// NOTE: should call after kinit
void init_kproc()
{
    // TODO:
    // 1. init global resources (e.g. locks, semaphores)
    // 2. init the root_proc (finished)
    // for(int i=0;i<NCPU;i++){
    //     init_spinlock(&plock[i]);
    // }
    init_spinlock(&pidlock);
    init_spinlock(&elock);
    init_spinlock(&klock);
    init_proc(&root_proc);
    root_proc.parent = &root_proc;
    start_proc(&root_proc, kernel_entry, 123456);
}

void init_proc(Proc *p)
{
    // TODO:
    // setup the Proc with kstack and pid allocated
    // NOTE: be careful of concurrency
    // 初始化为UNUSED状态
    // ucontext = base - 16 - sizeof(UserContext)
    // kcontext = base - 16 - sizeof(UserContext)-sizeof(KernelContext)
    memset(p,0,sizeof(*p));
    init_spinlock(&p->plock);
    acquire_spinlock(&p->plock);
    p->pid = get_pid();  // 使用get_pid()来获取PID
    init_sem(&p->childexit,0);
    init_pgdir(&p->pgdir);
    init_oftable(&p->oftable);
    init_list_node(&p->children);
    init_list_node(&p->ptnode);
    p->vma = NULL;
    p->killed = 0;
    p->idle = 0;
    p->wait_time =0;
    p->state = UNUSED;
    p->parent = NULL;
    p->kstack= kalloc_page();
    init_schinfo(&p->schinfo);
    p->kcontext = (KernelContext*)((u64)p->kstack + PAGE_SIZE - 16 - sizeof(KernelContext)-sizeof(UserContext));
    p->ucontext = (UserContext*)  ((u64)p->kstack + PAGE_SIZE - 16 - sizeof(UserContext));
    release_spinlock(&p->plock);
}

Proc *create_proc()
{
    Proc *p = kalloc(sizeof(Proc));
    init_proc(p);
    return p;
}

void set_parent_to_this(Proc *proc)
{
    // TODO: set the parent of proc to thisproc
    // NOTE: maybe you need to lock the process tree
    // NOTE: it's ensured that the old proc->parent = NULL
    //确保这个函数只使用在未start的进程上
    auto this = thisproc();
    acquire_spinlock(&proc->plock);
    if(proc->parent==NULL){
        proc->parent = thisproc();
        release_spinlock(&proc->plock);
        acquire_spinlock(&this->plock);
        _insert_into_list(&thisproc()->children,&proc->ptnode);
        release_spinlock(&this->plock);
        return;
    }
    PANIC();//只对新创建的进程使用
}

int start_proc(Proc *p, void (*entry)(u64), u64 arg)
{
    // TODO:
    // 1. set the parent to root_proc if NULL
    // 2. setup the kcontext to make the proc start with proc_entry(entry, arg)
    // 3. activate the proc and return its pid
    // NOTE: be careful of concurrency
    if(p->parent ==NULL ){
        
        p->parent = &root_proc;
        acquire_spinlock(&root_proc.plock);
        _insert_into_list(&root_proc.children,&p->ptnode);
        release_spinlock(&root_proc.plock);
        
    }
    p->kcontext->lr = (u64)&proc_entry;
    p->kcontext->x0 = (u64)entry;
    p->kcontext->x1 = (u64) arg;
    int id = p->pid;
    activate_proc(p);
    return id;
}

int wait(int *exitcode)
{
    auto this = thisproc();

    acquire_spinlock(&this->plock);
    if (_empty_list(&this->children)) {
        release_spinlock(&this->plock);
        return -1;
    }
    release_spinlock(&this->plock);

    bool x = wait_sem(&this->childexit);
    if (x == false) {
        return -1;
    }

    acquire_spinlock(&this->plock);
    auto node = this->children.next;
    while (node != &this->children) {
        Proc *x = container_of(node, Proc, ptnode);
        node = node->next;
        acquire_spinlock(&x->plock);
        if (x->state == ZOMBIE) {
            recycle_pid(x->pid);
            _detach_from_list(&x->ptnode);
            release_spinlock(&this->plock);


            int id = x->pid;
            *exitcode = x->exitcode;
            release_spinlock(&x->plock);
            acquire_sched_lock();
            _detach_from_list(&x->schinfo.rq);
            release_sched_lock();
            kfree_page(x->kstack);
            kfree(x);
            return id;
        }
        release_spinlock(&x->plock);
    }
    release_spinlock(&this->plock);
    return -1;
}


void reparent(Proc *this)
{
    if (_empty_list(&this->children))
        return;
    auto node = this->children.next;
    while (node != &this->children) {
        auto x = container_of(node, Proc, ptnode);
        node = node->next;

        acquire_spinlock(&x->plock);
        x->parent = &root_proc;
        release_spinlock(&x->plock);

        _detach_from_list(&x->ptnode);

        acquire_spinlock(&root_proc.plock);
        _insert_into_list(&root_proc.children, &x->ptnode);
        release_spinlock(&root_proc.plock);

        if (is_zombie(x)) {
            post_sem(&root_proc.childexit);
        }
    }
}

NO_RETURN void exit(int code)
{
    acquire_spinlock(&elock);
    auto this = thisproc();

    acquire_spinlock(&this->plock);

    reparent(this);
    
    this->exitcode = code;

    for (int i = 0; i < OPENFILE; ++i) {
        if (this->oftable.files[i]) {
            release_spinlock(&this->plock);
            file_close(this->oftable.files[i]);
            acquire_spinlock(&this->plock);
            this->oftable.files[i] = NULL;
        }
    }
    // munmap all mmap vma
    Vma* v  = this->vma;
    Vma* pv = NULL;
    release_spinlock(&elock);
    while(v){
        writeback(v,v->start,v->length);
        uvmunmap(&this->pgdir,v->start,PGROUNDUP(v->length)/PAGE_SIZE , true);
        file_close(v->file);
        pv = v->next;
        acquire_spinlock(&v->lock);
        v->next =NULL;
        v->length = 0;
        release_spinlock(&v->lock);
        v =pv;
    }
    acquire_spinlock(&elock);
    //kfree((void *)&this->oftable);
    if(this->cwd)
        inodes.put(NULL, this->cwd);
    this->cwd = NULL;
    free_pgdir(&this->pgdir);
    if(this->parent !=NULL){
        post_sem(&this->parent->childexit);
    }
    acquire_sched_lock();

    release_spinlock(&this->plock);
    release_spinlock(&elock);

    sched(ZOMBIE);

    PANIC(); 
}


Proc* dfs_kill(Proc *p, int pid)
{
    // 遍历当前进程的子进程
   if (p->pid == pid && !is_unused(p)){
        p->killed = 1;
        return p;
    }
    _for_in_list(c,&p->children){
        if(c == &p->children)
            continue;
        auto childproc = container_of( c , Proc , ptnode);
        Proc * k = dfs_kill(childproc,pid);
        if(k != NULL)
            return k;
    }
    return NULL;
}

int kill(int pid)
{
    // TODO:
    // Set the killed flag of the proc to true and return 0.
    // Return -1 if the pid is invalid (proc not found).
    acquire_spinlock(&klock); // 获取当前 CPU 的自旋锁
    Proc *k = dfs_kill(&root_proc,pid);       // 获取当前进程

    if(k !=NULL){
        if(((k -> ucontext -> elr)>>48)==0){
            alert_proc(k);
            return 0;
        }
        else return -1;
    }
    release_spinlock(&klock); // 释放自旋锁
    return -1;  // 如果 killed 为 true，则返回 0，表示成功；否则返回 -1
}

/*
 * Create a new process copying p as the parent.
 * Sets up stack to return as if from system call.
 */
void trap_return();
int fork()
{
    /**
     * (Final) TODO BEGIN
     * 
     * 1. Create a new child process.
     * 2. Copy the parent's memory space.
     * 3. Copy the parent's trapframe.
     * 4. Set the parent of the new proc to the parent of the parent.
     * 5. Set the state of the new proc to RUNNABLE.
     * 6. Activate the new proc and return its pid.
     */

    /* (Final) TODO END */
    //创建一个字进程
    Proc* current_proc=thisproc();
    Proc* new_proc=create_proc();
    if(!new_proc){
        return -1;
    }
    //2.复制页表
    struct pgdir * new_pgdir = uvmcopy(&(current_proc->pgdir));
    if(!new_pgdir){
        //如果页表获取失败，不能调用这个进程了...
        acquire_spinlock(&new_proc->plock);
        kfree(new_proc->kstack);
        new_proc->state = UNUSED;
        release_spinlock(&new_proc->plock);
        return -1;
    }
    new_proc->pgdir = * new_pgdir;
    //如果将子进程的parent设置为parent的parent，再init里面的wait会先检查有无子进程
    //此时原进程没有子进程，会直接返回-1，导致不断启动sh，这一要求似乎不太合理？
    //不太好改动exit的判断逻辑，因此修改fork的parent为原进程，这样就不会死循环了。
    new_proc->parent =current_proc;
    Proc *parent = current_proc;
    acquire_spinlock(&parent->plock);
    _insert_into_list(&parent->children,&new_proc->ptnode );
    release_spinlock(&parent->plock);
    // 上下文
    *(new_proc->ucontext) = *(current_proc->ucontext);
    // Cause fork to return 0 in the child.
    new_proc->ucontext->x[0] = 0;
    new_proc->cwd = inodes.share(current_proc->cwd);
    //打开文件
    for(int i =0;i<OPENFILE;i++){
        if((current_proc->oftable).files[i]){
            (new_proc->oftable).files[i] = file_dup((current_proc->oftable).files[i]);
        }
    }
    //路径
    int pid = new_proc->pid;
    // 复制vma
    new_proc->vma = NULL;
    Vma* pv = current_proc->vma;
    Vma* pre = NULL;
    while(pv){
        Vma* vma = vma_alloc();
        vma->start = pv->start;
        vma->end = pv->end;
        vma->off = pv->off;
        vma->length = pv->length;
        vma->permission = pv->permission;
        vma->flags = pv->flags;
        vma->file = pv->file;
        file_dup(vma->file);
        vma->next =NULL;
        if(pre == NULL){
            new_proc->vma = vma;
        }
        else{
            pre->next =vma;
        }
        pre = vma;
        release_spinlock(&vma->lock);
        pv = pv->next;
    }
    start_proc(new_proc,trap_return,0);
    return pid;
}


Vma* vma_alloc(){
  for(int i = 0; i < NVMA; i++){
    acquire_spinlock(&vma_list[i].lock);
    if(vma_list[i].length == 0){
      return &vma_list[i];
    }else{
      release_spinlock(&vma_list[i].lock);
    }
  }
  printk("no enough vma");
  PANIC();
}

void dfs(Proc* p,char ** states){
    char* state=NULL;
    if(p->state >= 0 && p->state < 6 && states[p->state])
      state = states[p->state];
    printk("%d %s\n", p->pid, state);
    auto c = (p->children).next;
    while(c != &(p->children)){
        auto childproc = container_of( c , Proc , ptnode);
        dfs(childproc,states);
        c = c->next;
    }
    return ;
}
void procdump(){
    //enum procstate { UNUSED, RUNNABLE, RUNNING, SLEEPING, DEEPSLEEPING, ZOMBIE };
    static char * states[]={
        [UNUSED]        "unused",
        [RUNNABLE]      "runnable",
        [RUNNING]       "running",
        [SLEEPING]      "sleep",
        [DEEPSLEEPING]  "deepsleep",
        [ZOMBIE]        "zombie"
    };
    Proc * p =&root_proc;
    printk("\n");
    //char * state=NULL;
    // printk("\n");
    //  if(p->state >= 0 && p->state < 6 && states[p->state])
    //   state = states[p->state];
    // printk("%d %s\n", p->pid, state);
    dfs(p,states);
    printk("$");
    //exit(0);
}