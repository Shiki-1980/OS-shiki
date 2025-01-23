#pragma once

#include <common/defines.h>
#include <common/list.h>
#include <common/sem.h>
#include <common/rbtree.h>
#include <kernel/pt.h>
#include <fs/file.h>
#include <fs/inode.h>

enum procstate { UNUSED, RUNNABLE, RUNNING, SLEEPING, DEEPSLEEPING, ZOMBIE };

#define NVMA 16
#define MAXVA (1L << (9 + 9 + 9 +1 - 1))
#define VMA_START (MAXVA >> 4)
#define PGROUNDUP(sz)  (((sz)+PAGE_SIZE-1) & ~(PAGE_SIZE-1))

typedef struct UserContext {
    u64 q0_0, q0_1;
    u64 elr;
    u64 tpidr;
    u64 sp;
    u64 spsr;
    u64 x[32];
} UserContext;

typedef struct vma{
  u64 start;
  u64 end;
  u64 length; // 0 means vma not used
  u64 off;
  int permission;
  int flags;
  File *file;
  struct vma *next;
  SpinLock lock;
}Vma;

typedef struct KernelContext {
    // TODO: customize your context
    u64 lr,x0,x1;
    u64 x[11]; //x19-29
} KernelContext;

// embeded data for procs
struct schinfo {
    // TODO: customize your sched info
    // 
    ListNode rq;//running runnable
};


typedef struct Proc {
    bool killed;
    bool idle;
    int pid;//可被回收复用
    int exitcode;
    int wait_time;
    enum procstate state;
    Semaphore childexit;
    SpinLock plock;//进程锁，猛猛加锁，解决并发问题
    ListNode children;
    ListNode ptnode;
    struct Proc *parent;
    struct schinfo schinfo;
    struct pgdir pgdir;
    void *kstack;
    UserContext *ucontext;
    KernelContext *kcontext;
    struct oftable oftable;
    Inode *cwd;
    Vma* vma;
} Proc;

typedef struct PidNode {
    int pid;
    struct PidNode *next;
} PidNode;
void init_kproc();
void init_proc(Proc *);
WARN_RESULT Proc *create_proc();
int start_proc(Proc *, void (*entry)(u64), u64 arg);
NO_RETURN void exit(int code);
WARN_RESULT int wait(int *exitcode);
WARN_RESULT int kill(int pid);
WARN_RESULT int fork();
void trap_return();

void set_parent_to_this(Proc *proc);

Vma* vma_alloc();

void procdump();
void dfs(Proc* p,char ** states);