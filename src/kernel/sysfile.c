//
// File-system system calls implementation.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/sysmacros.h>
#include <sys/mman.h>
#include <stddef.h>

#include "syscall.h"
#include <aarch64/mmu.h>
#include <common/defines.h>
#include <common/spinlock.h>
#include <common/string.h>
#include <fs/file.h>
#include <fs/fs.h>
#include <fs/inode.h>
#include <fs/pipe.h>
#include <kernel/mem.h>
#include <kernel/paging.h>
#include <kernel/printk.h>
#include <kernel/proc.h>
#include <kernel/sched.h>

struct iovec {
    void *iov_base; /* Starting address. */
    usize iov_len; /* Number of bytes to transfer. */
};

/** 
 * Get the file object by fd. Return null if the fd is invalid.
 */
static struct file *fd2file(int fd)
{
    /* (Final) TODO BEGIN */
    if(fd<0||fd>=OPENFILE)
        return NULL;
    return (thisproc()->oftable.files[fd]);
    /* (Final) TODO END */
}

/*
 * Allocate a file descriptor for the given file.
 * Takes over file reference from caller on success.
 */

int fdalloc(struct file *f)
{
    /* (Final) TODO BEGIN */
    Proc* p=thisproc();
    for(int fd=0;fd<OPENFILE;fd++){
       if (p->oftable.files[fd]==0){
            p->oftable.files[fd]=f;
            return fd;
        }
    }
    /* (Final) TODO END */
    return -1;
}
void writeback(struct vma* v, u64 addr, int n)
{
  if((v->permission & PTE_RO) || (v->flags & MAP_PRIVATE )) // no need to writeback
    return;

  if((addr % PAGE_SIZE) != 0){
    printk("unmap: not aligned");
    PANIC();
  }

  //printk("starting writeback: %p %d\n", (void *)addr, n);

  File* f = v->file;

  int max = ((OP_MAX_NUM_BLOCKS-1-1-2) / 2) * BLOCK_SIZE;
  int i = 0;
  while(i < n){
    int n1 = n - i;
    if(n1 > max)
      n1 = max;

    OpContext ctx;
    bcache.begin_op(&ctx);
    inodes.lock(f->ip);
    //printk("%p %lld %d\n",(void *)addr + i, v->off + v->start - addr, n1);
    int r = inodes.write(&ctx,f->ip ,(u8*)addr + i , v->off + v->start -addr +i ,n1);
    //int r = writei(f->ip, 1, addr + i, v->off + v->start - addr + i, n1);
    inodes.unlock(f->ip);
    bcache.end_op(&ctx);
    i += r;
  }
}
define_syscall(ioctl, int fd, u64 request)
{
    // 0x5413 is TIOCGWINSZ (I/O Control to Get the WINdow SIZe, a magic request
    // to get the stdin terminal size) in our implementation. Just ignore it.
    ASSERT(request == 0x5413);
    (void)fd;
    return 0;
}

define_syscall(mmap, void *addr, int length, int prot, int flags, int fd,
               int offset)
{
    /* (Final) TODO BEGIN */

    if(addr != 0){
        printk("mmap: addr not 0\n");
        PANIC();
    }
    if(offset != 0){
        printk("mmap: offset not 0\n");
        PANIC();
    }
    Proc* p = thisproc();
    File* f = p->oftable.files[fd];

    int pte_flag = PTE_USER_DATA;
    if(prot & PROT_WRITE){
        if(!f->writable && !(flags & MAP_PRIVATE))
            return -1;//尝试map一个不可写的file到可写的地方
        pte_flag |= PTE_RW;
    }
    if(prot & PROT_READ){
        if(!f->readable) 
            return -1; //尝试map一个不可读的file到可读的地方
        //pte_flag |= PTE_RO;

    }

    Vma* v = vma_alloc();
    v->permission = pte_flag;
    v->length =length;
    v->off =offset;
    v->file = thisproc()->oftable.files[fd];
    v->flags = flags;
    file_dup(f);
    Vma* pv = p->vma;
    if(pv == 0 || pv -> length ==0){
        v->start =VMA_START;
        v->end = v->start + length;
        p->vma = v;
    }
    else{
        while(pv->next)
            pv=pv->next;
        u64 temp_end= pv->end ;
        v->start = PAGE_BASE((temp_end + PAGE_SIZE - 1));
        v->end = v->start + length;
        pv->next = v;
        v->next =NULL;
    }
    // TODO : 在映射的同时把文件的内容也映射过来，不要mmap_handler(
    //
    addr = (void*) v->start;
    printk("mmap: [%p, %p)\n", addr, (void *)v->end);
    release_spinlock(&v->lock);
    return (u64)addr; 
    /* (Final) TODO END */
}

define_syscall(munmap, void *addr, size_t length)
{
    /* (Final) TODO BEGIN */
    Proc* p = thisproc();
    Vma* v = p->vma;
    Vma* pre = NULL;
    if(v == 0)
        return -1;
    while(v!=NULL){
       if((u64)addr >= v->start && (u64)addr < v->end)
        break;// 找到合适的
     pre = v;
     v=v->next;
    }

    if( v==NULL) return -1;
    printk("munmap: %p  %ld\n",addr,length);
    if((u64)addr != v->start && (u64)addr + length != v->end){
        printk("munmap middle of vma\n");
        PANIC();
    }
    if((u64)addr == v->start ){
         writeback(v, (u64)addr, length);
         uvmunmap(&p->pgdir,(u64)addr,length/PAGE_SIZE,1);
         if(length == v->length){
            file_close(v->file);
            if(pre == 0){
                p->vma = v->next;
            }
            else{
                pre->next = v->next;
                v->next = NULL;
            }
            acquire_spinlock(&v->lock);
            v->length =0;
            release_spinlock(&v->lock);
         }else{
            //不是头部还要再判断

            //v->start -= length;
            v->off += length;
            v->length -= length;
         }
    }
    else{
        v->length -= length;
        v->end -= length;
    }
    return 0;
    /* (Final) TODO END */
}

define_syscall(dup, int fd)
{
    struct file *f = fd2file(fd);
    if (!f)
        return -1;
    fd = fdalloc(f);
    if (fd < 0)
        return -1;
    file_dup(f);
    return fd;
}

define_syscall(read, int fd, char *buffer, int size)
{
    struct file *f = fd2file(fd);
    if (!f || size <= 0 || !user_writeable(buffer, size))
        return -1;
    return file_read(f, buffer, size);
}

define_syscall(write, int fd, char *buffer, int size)
{
    struct file *f = fd2file(fd);
    if (!f || size <= 0 || !user_readable(buffer, size))
        return -1;
    return file_write(f, buffer, size);
}

define_syscall(writev, int fd, struct iovec *iov, int iovcnt)
{
    struct file *f = fd2file(fd);
    struct iovec *p;
    if (!f || iovcnt <= 0 || !user_readable(iov, sizeof(struct iovec) * iovcnt))
        return -1;
    usize tot = 0;
    for (p = iov; p < iov + iovcnt; p++) {
        if (!user_readable(p->iov_base, p->iov_len))
            return -1;
        tot += file_write(f, p->iov_base, p->iov_len);
    }
    return tot;
}

define_syscall(close, int fd)
{
    /* (Final) TODO BEGIN */
    File* f=fd2file(fd);
    if (f == NULL) {
        return -1;
    }
    thisproc()->oftable.files[fd]=NULL;
    file_close(f);
    /* (Final) TODO END */
    return 0;
}

define_syscall(fstat, int fd, struct stat *st)
{
    struct file *f = fd2file(fd);
    if (!f || !user_writeable(st, sizeof(*st)))
        return -1;
    return file_stat(f, st);
}

define_syscall(newfstatat, int dirfd, const char *path, struct stat *st,
               int flags)
{
    if (!user_strlen(path, 256) || !user_writeable(st, sizeof(*st)))
        return -1;
    if (dirfd != AT_FDCWD) {
        printk("sys_fstatat: dirfd unimplemented\n");
        return -1;
    }
    if (flags != 0) {
        printk("sys_fstatat: flags unimplemented\n");
        return -1;
    }

    Inode *ip;
    OpContext ctx;
    bcache.begin_op(&ctx);
    if ((ip = namei(path, &ctx)) == 0) {
        bcache.end_op(&ctx);
        return -1;
    }
    inodes.lock(ip);
    stati(ip, st);
    inodes.unlock(ip);
    inodes.put(&ctx, ip);
    bcache.end_op(&ctx);

    return 0;
}

static int isdirempty(Inode *dp)
{
    usize off;
    DirEntry de;

    for (off = 2 * sizeof(de); off < dp->entry.num_bytes; off += sizeof(de)) {
        if (inodes.read(dp, (u8 *)&de, off, sizeof(de)) != sizeof(de))
            PANIC();
        if (de.inode_no != 0)
            return 0;
    }
    return 1;
}

define_syscall(unlinkat, int fd, const char *path, int flag)
{
    ASSERT(fd == AT_FDCWD && flag == 0);
    Inode *ip, *dp;
    DirEntry de;
    char name[FILE_NAME_MAX_LENGTH];
    usize off;
    if (!user_strlen(path, 256))
        return -1;
    OpContext ctx;
    bcache.begin_op(&ctx);
    if ((dp = nameiparent(path, name, &ctx)) == 0) {
        bcache.end_op(&ctx);
        return -1;
    }

    inodes.lock(dp);

    // Cannot unlink "." or "..".
    if (strncmp(name, ".", FILE_NAME_MAX_LENGTH) == 0 ||
        strncmp(name, "..", FILE_NAME_MAX_LENGTH) == 0)
        goto bad;

    usize inumber = inodes.lookup(dp, name, &off);
    if (inumber == 0)
        goto bad;
    ip = inodes.get(inumber);
    inodes.lock(ip);

    if (ip->entry.num_links < 1)
        PANIC();
    if (ip->entry.type == INODE_DIRECTORY && !isdirempty(ip)) {
        inodes.unlock(ip);
        inodes.put(&ctx, ip);
        goto bad;
    }

    memset(&de, 0, sizeof(de));
    if (inodes.write(&ctx, dp, (u8 *)&de, off, sizeof(de)) != sizeof(de))
        PANIC();
    if (ip->entry.type == INODE_DIRECTORY) {
        dp->entry.num_links--;
        inodes.sync(&ctx, dp, true);
    }
    inodes.unlock(dp);
    inodes.put(&ctx, dp);
    ip->entry.num_links--;
    inodes.sync(&ctx, ip, true);
    inodes.unlock(ip);
    inodes.put(&ctx, ip);
    bcache.end_op(&ctx);
    return 0;

bad:
    inodes.unlock(dp);
    inodes.put(&ctx, dp);
    bcache.end_op(&ctx);
    return -1;
}

/**
    @brief create an inode at `path` with `type`.

    If the inode exists, just return it.

    If `type` is directory, you should also create "." and ".." entries and link
   them with the new inode.

    @note BE careful of handling error! You should clean up ALL the resources
   you allocated and free ALL acquired locks when error occurs. e.g. if you
   allocate a new inode "/my/dir", but failed to create ".", you should free the
   inode "/my/dir" before return.

    @see `nameiparent` will find the parent directory of `path`.

    @return Inode* the created inode, or NULL if failed.
 */
Inode *create(const char *path, short type, short major, short minor, OpContext *ctx)
{
    char name[FILE_NAME_MAX_LENGTH];
    Inode *parent_inode;
    Inode *ip= NULL;
    parent_inode = nameiparent(path,name ,ctx);
    if (parent_inode == NULL) {
        return NULL;
    }

    // 检查父目录中是否已经存在同名的条目
    auto new_dentry = inodes.lookup(parent_inode, name, 0);
    ip = inodes.get(new_dentry);
    if (ip != NULL) {
        // 已经有了，不用创建，解锁目录
        inodes.unlock(parent_inode);
        inodes.put(ctx,parent_inode);
        // 检查ip
        inodes.lock(ip);
        if(type == INODE_REGULAR &&
           ip->entry.type == INODE_REGULAR){
            //持有锁直接返回
           return ip;
           }
        //类型不对，有问题
        inodes.unlock(ip);
        inodes.put(ctx,ip);
        return NULL;
    }
    //没有，新建一个inode
    ip = inodes.get(inodes.alloc(ctx,type));
    if(ip==NULL){
        inodes.put(ctx,parent_inode);
        return NULL;
    }
    inodes.lock(ip);
    ip->entry.major=major;
    ip->entry.minor=minor;
    ip->entry.num_links =1;
    inodes.sync(ctx,ip,true);//更新inode
    //如果类型是目录，需要处理.和..
    if(ip->entry.type == INODE_DIRECTORY){
        //父目录链接数增加
        parent_inode->entry.num_links ++;
        // 每次更改inode都要调用sync
        inodes.sync(ctx,parent_inode,true);
        inodes.insert(ctx,ip,".",ip->inode_no);// 。目录对应自己
        inodes.insert(ctx,ip,"..",parent_inode->inode_no); // .. 对应父目录
    }
    inodes.insert(ctx,parent_inode,name,ip->inode_no); // 把新建的inode插到父目录下
    inodes.unlock(parent_inode);
    inodes.put(ctx,parent_inode);
    return ip;
}
define_syscall(openat, int dirfd, const char *path, int omode)
{
    int fd;
    struct file *f;
    Inode *ip;

    if (!user_strlen(path, 256))
        return -1;

    if (dirfd != AT_FDCWD) {
        printk("sys_openat: dirfd unimplemented\n");
        return -1;
    }

    OpContext ctx;
    bcache.begin_op(&ctx);
    if (omode & O_CREAT) {
        // FIXME: Support acl mode.
        ip = create(path, INODE_REGULAR, 0, 0, &ctx);
        if (ip == 0) {
            bcache.end_op(&ctx);
            return -1;
        }
    } else {
        if ((ip = namei(path, &ctx)) == 0) {
            bcache.end_op(&ctx);
            return -1;
        }
        inodes.lock(ip);
    }

    if ((f = file_alloc()) == 0 || (fd = fdalloc(f)) < 0) {
        if (f)
            file_close(f);
        inodes.unlock(ip);
        inodes.put(&ctx, ip);
        bcache.end_op(&ctx);
        return -1;
    }
    inodes.unlock(ip);
    bcache.end_op(&ctx);

    f->type = FD_INODE;
    f->ip = ip;
    f->off = 0;
    f->readable = !(omode & O_WRONLY);
    f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
    return fd;
}

define_syscall(mkdirat, int dirfd, const char *path, int mode)
{
    Inode *ip;
    if (!user_strlen(path, 256))
        return -1;
    if (dirfd != AT_FDCWD) {
        printk("sys_mkdirat: dirfd unimplemented\n");
        return -1;
    }
    if (mode != 0) {
        printk("sys_mkdirat: mode unimplemented\n");
        return -1;
    }
    OpContext ctx;
    bcache.begin_op(&ctx);
    if ((ip = create(path, INODE_DIRECTORY, 0, 0, &ctx)) == 0) {
        bcache.end_op(&ctx);
        return -1;
    }
    inodes.unlock(ip);
    inodes.put(&ctx, ip);
    bcache.end_op(&ctx);
    return 0;
}

define_syscall(mknodat, int dirfd, const char *path, dev_t dev)
{
    
    Inode *ip;
    if (!user_strlen(path, 256))
        return -1;
    if (dirfd != AT_FDCWD) {
        printk("sys_mknodat: dirfd unimplemented\n");
        return -1;
    }

    unsigned int ma = major(dev);
    unsigned int mi = minor(dev);
    printk("mknodat: path '%s', major:minor %u:%u\n", path, ma, mi);
    OpContext ctx;
    bcache.begin_op(&ctx);
    if ((ip = create(path, INODE_DEVICE, (short)ma, (short)mi, &ctx)) == 0) {
        bcache.end_op(&ctx);
        return -1;
    }
    inodes.unlock(ip);
    inodes.put(&ctx, ip);
    bcache.end_op(&ctx);
    return 0;
}
define_syscall(chdir, const char *path)
{
    /**
     * (Final) TODO BEGIN 
     * 
     * Change the cwd (current working dictionary) of current process to 'path'.
     * You may need to do some validations.
     */
    Inode * ip ;
    Proc * cur = thisproc();//当前进程
    OpContext ctx;
    bcache.begin_op(&ctx);
    ip = namei(path,&ctx);
    if(ip ==NULL){
        printk("Error: The path(%s) does not exist.\n", path);
        bcache.end_op(&ctx);
        return -1; 
    }
    inodes.lock(ip);
    if(ip->entry.type !=INODE_DIRECTORY){
        //打开的不是文件夹
        inodes.unlock(ip);
        inodes.put(&ctx,ip);
        bcache.end_op(&ctx);
        printk("Error: The path(%s) is not a directory.\n", path);
        return -1;
    }
    inodes.unlock(ip);
    inodes.put(&ctx,ip);
    bcache.end_op(&ctx);
    cur->cwd = ip;
    return 0;
    /* (Final) TODO END */
}

define_syscall(pipe2, int pipefd[2], int flags)
{

    /* (Final) TODO BEGIN */
    File *rf, * wf;
    Proc *p = thisproc();
    flags = flags ;
    if(pipe_alloc(&rf,&wf)<0) 
        return -1;
    int fd0 = fdalloc(rf);
    int fd1 = fdalloc(wf);
    if(fd0 == -1 || fd1 == -1){
        if(fd0 != -1){
            p->oftable.files[fd0] = NULL;
            file_close(rf);
        }
        if(fd1 != -1){
            p->oftable.files[fd1] = NULL;
            file_close(wf);
        }
        return -1;
    }
    pipefd[0] = fd0;
    pipefd[1] = fd1;

    return 0;
    /* (Final) TODO END */
}