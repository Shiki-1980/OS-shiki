#include <kernel/mem.h>
#include <kernel/sched.h>
#include <fs/pipe.h>
#include <common/string.h>
#include <kernel/printk.h>

void init_pipe(Pipe *pi)
{
    /* (Final) TODO BEGIN */
    memset(pi,0,sizeof(Pipe));
    init_spinlock(&pi->lock);
    init_sem(&pi->wlock,0);
    init_sem(&pi->rlock,0);
    pi->nread      = 0;
    pi->nwrite    = 0;
    pi->readopen  = 1;
    pi->writeopen = 1;
    /* (Final) TODO END */
}

void init_read_pipe(File *readp, Pipe *pipe)
{
    /* (Final) TODO BEGIN */
    readp -> type = FD_PIPE;
    readp -> readable = 1;
    readp -> writable = 0;
    readp->pipe = pipe;
    /* (Final) TODO END */
}

void init_write_pipe(File *writep, Pipe *pipe)
{
    /* (Final) TODO BEGIN */
    writep -> type = FD_PIPE;
    writep -> readable = 0;
    writep -> writable = 1;
    writep->pipe = pipe;
    /* (Final) TODO END */
}

int pipe_alloc(File **f0, File **f1)
{
    /* (Final) TODO BEGIN */
    Pipe * pi;

    pi = 0;
    *f0 = *f1 =0;
    if((*f0 = file_alloc()) == 0 || (*f1 = file_alloc())==0)
        goto bad;
    if((pi= (Pipe *)kalloc(sizeof(Pipe)))==0)
        goto bad;
    init_pipe(pi);
    init_read_pipe (*f0,pi);
    init_write_pipe(*f1,pi);
    return 0;

bad:
    if(pi){
        kfree((void*) pi);
    }
    if(*f0){
        file_close(*f0);
    }
    if(*f1){
        file_close(*f1);
    }
    return -1;
    /* (Final) TODO END */
}

void pipe_close(Pipe *pi, int writable)
{
    /* (Final) TODO BEGIN */
    acquire_spinlock(&pi->lock);
    if(writable){
        pi->writeopen = 0;
        post_all_sem(&pi->rlock);
    }
    if(!writable){
        pi->readopen = 0;
        post_all_sem(&pi->wlock);
    }
    if(pi->readopen == 0 && pi->writeopen == 0){
        release_spinlock(&pi->lock);
        kfree((void *)pi);
    }
    else{
    release_spinlock(&pi->lock);
    }
    /* (Final) TODO END */
}

int pipe_write(Pipe *pi, u64 addr, int n)
{
    /* (Final) TODO BEGIN */
    char* src = (char*)addr;
    int i;
    acquire_spinlock(&(pi->lock));
     for (i=0;i<n;i++){
        while (pi->nwrite==pi->nread+PIPE_SIZE){
            if (pi->readopen==0||thisproc()->killed){
                release_spinlock(&pi->lock);
                return -1;
            }
            post_sem(&pi->rlock);
            release_spinlock(&pi->lock);
            unalertable_wait_sem(&pi->wlock);
        }
        pi->data[pi->nwrite++ % PIPE_SIZE] = src[i];
    }
    post_all_sem(&(pi->rlock));
    release_spinlock(&(pi->lock));
    return i;
    /* (Final) TODO END */
}

int pipe_read(Pipe *pi, u64 addr, int n)
{
    /* (Final) TODO BEGIN */
     char* dst = (char*)addr;
    
    acquire_spinlock(&(pi->lock));
     while (pi->nread==pi->nwrite&&pi->writeopen){
        if (thisproc()->killed){
            release_spinlock(&pi->lock);
            return -1;
        }
        release_spinlock(&pi->lock);
        unalertable_wait_sem(&pi->rlock);
    }
    int i;
    for (i=0;i<n;i++){
        if (pi->nread==pi->nwrite) break;
        dst[i] = pi->data[pi->nread++ % PIPE_SIZE];
    }
    
    release_spinlock(&(pi->lock));
    post_all_sem(&(pi->wlock));
    
    return i;
    /* (Final) TODO END */
}