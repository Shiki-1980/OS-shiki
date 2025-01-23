#include <kernel/sched.h>
#include <kernel/proc.h>
#include <kernel/mem.h>
#include <kernel/printk.h>
#include <aarch64/intrinsic.h>
#include <kernel/cpu.h>
#include <common/rbtree.h>
#include <common/spinlock.h>
#include <common/list.h>

extern bool panic_flag;

extern void swtch(KernelContext *new_ctx, KernelContext **old_ctx);

static SpinLock rqlock;
static ListNode rq;
static struct timer sched_timer[NCPU];

void init_sched()
{
    // TODO: initialize the scheduler
    // 1. initialize the resources (e.g. locks, semaphores)
    // 2. initialize the scheduler info of each CPU
    init_spinlock(&rqlock);
    init_list_node(&rq);
    for(int i =0;i<NCPU;i++){
        struct Proc* p  =kalloc(sizeof(struct Proc));
        p->idle = 1;
        p->state = RUNNING;
        cpus[i].sched.thisproc = cpus[i].sched.idle = p;
        sched_timer[i].triggered=true;
        sched_timer[i].data=i;
        sched_timer[i].elapse=15;
        sched_timer[i].handler=&sched_timer_handler;
    }
}

void sched_timer_handler(struct timer* tim){
    tim->triggered=false;
    acquire_sched_lock();
    sched(RUNNABLE);
}   

Proc *thisproc()
{
    // TODO: return the current process
    return cpus[cpuid()].sched.thisproc;
}

void init_schinfo(struct schinfo *p)
{
    // TODO: initialize your customized schinfo for every newly-created process
    init_list_node(&p->rq);
}

void acquire_sched_lock()
{
    // TODO: acquire the sched_lock if need
    acquire_spinlock(&rqlock);
}

void release_sched_lock()
{
    // TODO: release the sched_lock if need
    release_spinlock(&rqlock);
}

bool is_zombie(Proc *p)
{
    bool r;
    acquire_sched_lock();
    r = p->state == ZOMBIE;
    release_sched_lock();
    return r;
}

bool is_unused(Proc *p)
{
    bool r;
    acquire_sched_lock();
    r = p->state == UNUSED;
    release_sched_lock();
    return r;
}

bool _activate_proc(Proc *p, bool onalert)
{
    // TODO:(Lab5 new)
    // if the proc->state is RUNNING/RUNNABLE, do nothing and return false
    // if the proc->state is SLEEPING/UNUSED, set the process state to RUNNABLE, add it to the sched queue, and return true
    // if the proc->state is DEEPSLEEPING, do nothing if onalert or activate it if else, and return the corresponding value.
    acquire_sched_lock();

    // 如果进程的状态为 RUNNING 或 RUNNABLE，则不执行任何操作，返回 false
    if (p->state == RUNNING || p->state == RUNNABLE||p->state==ZOMBIE||(p->state == DEEPSLEEPING && onalert)) {
        release_sched_lock();
        return false;
    }

    // 如果进程的状态为 SLEEPING 或 UNUSED，将其状态设置为 RUNNABLE，
    // 重置等待时间，将其加入调度队列并返回 true
    if (p->state == SLEEPING || p->state == UNUSED||(p->state == DEEPSLEEPING && !onalert)) {
        p->state = RUNNABLE;
        p->wait_time = 0;
        _insert_into_list(&rq, &p->schinfo.rq);  // 加入调度队列
        release_sched_lock();
        return true;
    }

    // 如果状态不在以上范围内，触发 PANIC
    release_sched_lock();
    PANIC();

    return false;  // 作为防御性返回
    
}

static void update_this_state(enum procstate new_state)
{
    // TODO: if you use template sched function, you should implement this routinue
    // update the state of current process to new_state, and modify the sched queue if necessary
        // 更新当前进程的状态

    Proc *p=thisproc();
    // 如果新状态是SLEEPING或者ZOMBIE，需要将其从调度队列中移除
    if (p!=cpus[cpuid()].sched.idle&&(p->state==RUNNING||p->state==RUNNABLE)){
        _detach_from_list(&p->schinfo.rq);
    }
    p->state = new_state;
    if(thisproc()!=cpus[cpuid()].sched.idle&&(new_state == RUNNING || new_state == RUNNABLE)){
        thisproc()->wait_time =0;
        _insert_into_list(&rq,&p->schinfo.rq);
    }
    if (!sched_timer[cpuid()].triggered){
        cancel_cpu_timer(&sched_timer[cpuid()]);
    }
    set_cpu_timer(&sched_timer[cpuid()]);

}

static Proc *pick_next()
{
    Proc *chosen_proc = NULL;
    int max_wait_time = -1;

    _for_in_list(p, &rq) {
        auto proc = container_of(p, struct Proc, schinfo.rq);
        
        // 更新等待时间并选择等待时间最长的进程
        if (proc->state == RUNNABLE) {
            proc->wait_time++;

            if (proc->wait_time > max_wait_time) {
                max_wait_time = proc->wait_time;
                chosen_proc = proc;
            }
        }
    }

    // 如果没有找到合适的进程，返回 idle 进程
    return chosen_proc ? chosen_proc : cpus[cpuid()].sched.idle;
}

static void update_this_proc(Proc *p)
{
    // TODO: you should implement this routinue
    // update thisproc to the choosen process
    //reset_clock(1000);//重置时钟中断，保证享有一个完整的时间片
    cpus[cpuid()].sched.thisproc = p;

}

// A simple scheduler.
// You are allowed to replace it with whatever you like.
// call with sched_lock
void sched(enum procstate new_state)
{
    //TODO:修改你的sched代码，使其能够保证，如果当前进程带有killed标记，且new state不为zombie，则调度器直接返回，不做任何操作。
    auto this = thisproc();
    ASSERT(this->state == RUNNING);
     // 检查当前进程是否被标记为 killed 且 new_state 不是 ZOMBIE
    if (this->killed && new_state != ZOMBIE) {
        // 如果当前进程被杀死且新状态不是 ZOMBIE，直接返回
        release_sched_lock();
        return;  // 直接返回，不做任何操作
    }
    update_this_state(new_state);
    auto next = pick_next();
    update_this_proc(next);
    ASSERT(next->state == RUNNABLE);
    next->state = RUNNING;
    if (next != this) {
        attach_pgdir(&next->pgdir);
        swtch(next->kcontext, &this->kcontext);
    }
    release_sched_lock();
}

u64 proc_entry(void (*entry)(u64), u64 arg)
{
    release_sched_lock();
    set_return_addr(entry);
    return arg;
}
