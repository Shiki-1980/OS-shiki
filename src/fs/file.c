#include "file.h"
#include <common/defines.h>
#include <common/spinlock.h>
#include <common/sem.h>
#include <common/string.h>
#include <fs/inode.h>
#include <common/list.h>
#include <kernel/mem.h>
#include <fs/pipe.h>
#include <stdio.h>
#include <kernel/proc.h>

// the global file table.
static struct ftable ftable;

void init_ftable() {
    // TODO: initialize your ftable.
    init_spinlock(&ftable.lock);
}

void init_oftable(struct oftable *oftable) {
    // TODO: initialize your oftable for a new process.
    for (int i=0;i<OPENFILE;i++) 
        oftable->files[i]=NULL;
}

/* Allocate a file structure. */
struct file* file_alloc() {
    /* (Final) TODO BEGIN */
    //获取文件表的锁
    acquire_spinlock(&ftable.lock);
     for(int i=0;i<NFILE;i++){
        //如果ref为0，则表示该项未被占用，可以分配
        if (ftable.files[i].ref==0){
            ftable.files[i].ref=1;
            release_spinlock(&ftable.lock);
            //返回文件结构的首地址
            return &(ftable.files[i]);
        }
    }
    release_spinlock(&ftable.lock);
    /* (Final) TODO END */
    return 0;
}

/* Increment ref count for file f. */
struct file* file_dup(struct file* f) {
    /* (Final) TODO BEGIN */
    acquire_spinlock(&ftable.lock);
    if(f->ref<1){
        //printk("文件分配错误，ref为0却调用了dup\n");
        release_spinlock(&ftable.lock);
        return f;
    }
    f->ref+=1;
    release_spinlock(&ftable.lock);
    /* (Final) TODO END */
    return f;
}

/* Close file f. (Decrement ref count, close when reaches 0.) */
void file_close(struct file* f) {
    /* (Final) TODO BEGIN */
    if(f==NULL)
        return ;
    acquire_spinlock(&ftable.lock);
    f->ref--;
    if(f->ref==0){
        File temp = *f;//记录关闭的file的类型
        memset(f, 0, sizeof(File));
        release_spinlock(&ftable.lock);
        if(temp.type == FD_PIPE){
            pipe_close(temp.pipe,temp.writable);
        }
        //对于inode类型的文件，需要释放与该文件关联的inode
        if(temp.type == FD_INODE){
            OpContext ctx;
            bcache.begin_op(&ctx);
            inodes.put(&ctx,temp.ip);
            bcache.end_op(&ctx);

        }
    }
    release_spinlock(&ftable.lock);
    /* (Final) TODO END */
}

/* Get metadata about file f. */
int file_stat(struct file* f, struct stat* st) {
    /* (Final) TODO BEGIN */
    Inode* ip = NULL;

    // 检查文件指针是否有效
    if (f == NULL || f->ip == NULL) {
        return -1; // 文件指针无效
    }

    ip = f->ip;
    if(f->type == FD_INODE){
    inodes.lock(ip);
    // 调用 stati 填充 stat 结构体
    stati(ip, st);
    inodes.unlock(ip);
    }
    return 0;
    /* (Final) TODO END */
}

/* Read from file f. */
isize file_read(struct file* f, char* addr, isize n) {
    /* (Final) TODO BEGIN */
    if(!f->readable)
        return  -1;//文件不可读
    
    else if( f->type == FD_PIPE){
        return (isize) pipe_read(f->pipe,(u64)addr ,n);
    }
    else if(f->type == FD_INODE){
        //调用inode里面的read读就行
        isize buf=0;
        Inode* ip = f->ip;
        inodes.lock(ip);
        buf = inodes.read(ip,(u8 *)addr,f->off,n);
        //别忘了更新f中的偏移！！！
        //还要判断buf是有效的长度....
        if(buf>0)
            f->off+=buf;
        inodes.unlock(ip);
        return buf;

    }
    //printf("file_read中， 文件类型错误\n");
    /* (Final) TODO END */
    return -1;
}

/* Write to file f. */
isize file_write(struct file* f, char* addr, isize n) {
    /* (Final) TODO BEGIN */
    
    if(!f->writable)
        return -1;//文件不可写
    else if(f->type == FD_PIPE){
        return (isize)pipe_write(f->pipe,(u64)addr ,n);
    }
    else if(f->type == FD_INODE){
        isize cur = 0;//记录写到哪儿了
        isize max_len =((OP_MAX_NUM_BLOCKS-4)/2) *BLOCK_SIZE;
        Inode* ip = f->ip;
        while(cur < n) {
            OpContext ctx;
            isize len=MIN(n-cur,max_len);
            bcache.begin_op(&ctx);
            inodes.lock(ip);
            isize wlen = inodes.write(&ctx,ip,(u8 *)(addr+cur),f->off,len);
            if(wlen>0)
                f->off += wlen;//成功写入了，下次文件中的偏移加一下
            inodes.unlock(ip);
            bcache.end_op(&ctx);
            if(len<0) break;
            cur += wlen;//更新src的起点
        }
        if(cur == n ) return n;//正确写入！
        return -1;//failed
    }
    /* (Final) TODO END */
    return 0;
}