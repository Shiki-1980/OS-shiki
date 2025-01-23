#include <common/string.h>
#include <fs/inode.h>
#include <kernel/mem.h>
#include <kernel/printk.h>
#include <common/rc.h>
#include<kernel/sched.h>
#include <kernel/console.h>

/**
    @brief the private reference to the super block.

    @note we need these two variables because we allow the caller to
            specify the block cache and super block to use.
            Correspondingly, you should NEVER use global instance of
            them.

    @see init_inodes
 */
static const SuperBlock* sblock;

/**
    @brief the reference to the underlying block cache.
 */
static const BlockCache* cache;

/**
    @brief global lock for inode layer.

    Use it to protect anything you need.

    e.g. the list of allocated blocks, ref counts, etc.
 */
static SpinLock lock;

/**
    @brief the list of all allocated in-memory inodes.

    We use a linked list to manage all allocated inodes.

    You can implement your own data structure if you want better performance.

    @see Inode
 */
static ListNode head;


// return which block `inode_no` lives on.
static INLINE usize to_block_no(usize inode_no) {
    return sblock->inode_start + (inode_no / (INODE_PER_BLOCK));
}

// return the pointer to on-disk inode.
static INLINE InodeEntry* get_entry(Block* block, usize inode_no) {
    return ((InodeEntry*)block->data) + (inode_no % INODE_PER_BLOCK);
}

// return address array in indirect block.
static INLINE u32* get_addrs(Block* block) {
    return ((IndirectBlock*)block->data)->addrs;
}

// initialize inode tree.
void init_inodes(const SuperBlock* _sblock, const BlockCache* _cache) {
    init_spinlock(&lock);
    init_list_node(&head);
    sblock = _sblock;
    cache = _cache;

    if (ROOT_INODE_NO < sblock->num_inodes)
        inodes.root = inodes.get(ROOT_INODE_NO);
    else
        printk("(warn) init_inodes: no root inode.\n");
}

// initialize in-memory inode.
static void init_inode(Inode* inode) {
    init_sleeplock(&inode->lock);
    init_rc(&inode->rc);
    init_list_node(&inode->node);
    inode->inode_no = 0;
    inode->valid = false;
}

// see `inode.h`.
static usize inode_alloc(OpContext* ctx, InodeType type) {
    ASSERT(type != INODE_INVALID);

    // 遍历所有 inode，查找空闲 inode
    for (usize ino = 1; ino < sblock->num_inodes; ino++) {
        // 计算当前 inode 对应的块号
        usize bno = to_block_no(ino);
        
        // 打印块号
        //printk("Checking inode %zu: blockno = %zu\n", ino, bno);
        
        // 获取当前块
        Block* now_block = cache->acquire(bno);
        
        // 获取当前 inode 对应的 InodeEntry
        InodeEntry* entry = get_entry(now_block, ino);
        
        // 打印 inode 的 type
        //printk("Inode %zu type = %d\n", ino, entry->type);
        
        // 如果 inode 是未使用的，则分配 inode
        if (entry->type == INODE_INVALID) {
            //printk("Inode %zu is free, allocating...\n", ino);
            memset(entry, 0, sizeof(InodeEntry));
            entry->type = type;  // 设置 inode 类型
            cache->sync(ctx, now_block);  // 同步数据
            cache->release(now_block);  // 释放块
            //printk("Allocated inode %zu with type %d\n", ino, type);
            return ino;  // 返回分配的 inode 编号
        }

        // 释放当前块
        cache->release(now_block);
    }

    // 如果没有找到可用的 inode，则触发 panic
    PANIC();
    return 0;
}


// see `inode.h`.
static void inode_lock(Inode* inode) {
    ASSERT(inode->rc.count > 0);
    // TODO
    //深度睡眠，避免不必要的中断
    unalertable_acquire_sleeplock(&inode->lock);
}

// see `inode.h`.
static void inode_unlock(Inode* inode) {
    ASSERT(inode->rc.count > 0);
    // TODO
    post_sem(&inode->lock);
}

// see `inode.h`.
static void inode_sync(OpContext* ctx, Inode* inode, bool do_write) {
    // TODO
    ASSERT(inode != NULL);  // 确保传入的inode有效

    // 计算inode所在的磁盘块编号
    usize block_no = to_block_no(inode->inode_no);
    Block* block = cache->acquire(block_no);  // 获取对应块的数据
    //indoe已从磁盘中读出，则写回磁盘
    if (inode->valid&&do_write) {
        // 写操作：将inode的数据写回磁盘
        InodeEntry* entry = get_entry(block, inode->inode_no);
        // 更新inode数据
        memcpy(entry,&inode->entry,sizeof(InodeEntry));
        // 同步到磁盘，确保数据持久化
        cache->sync(ctx, block);  // 假设 sync会将数据写回磁盘

    } 
    //inode还没用从磁盘中读出
    if(!inode->valid) {
        // 读操作：从磁盘读取inode的数据
        InodeEntry* entry = get_entry(block, inode->inode_no);
        memcpy(&inode->entry,entry,sizeof(InodeEntry));
        // 将磁盘数据加载到内存中的inode结构
        inode->valid = true;
    }

    // 释放缓存块
    cache->release(block);
}

// see `inode.h`.
static Inode* inode_get(usize inode_no) {
    if (inode_no==0){
        return NULL;
    }
    ASSERT(inode_no > 0);
    ASSERT(inode_no < sblock->num_inodes);
    // TODO


    // 遍历 inode 链表，查找是否已经存在该 inode
    acquire_spinlock(&lock);
    _for_in_list(p,&head){
        if (p==&head) continue;
        auto now=container_of(p,Inode,node);
        if (now->inode_no==inode_no){
            increment_rc(&now->rc);
            release_spinlock(&lock);
            return now;
        }
    }
    //没找到，分配新的
    Inode * new = kalloc(sizeof(Inode));
    init_inode(new);
    new->inode_no = inode_no;
    increment_rc(&new->rc);
    inode_lock(new);
    inode_sync(NULL,new,false);//直接读取就好
    inode_unlock(new);
    _insert_into_list(&head,&new->node);//加入inode链表
    release_spinlock(&lock);
    return new;
}
// see `inode.h`.
static void inode_clear(OpContext* ctx, Inode* inode) {
    // TODO
    ASSERT(inode!=NULL);
    //先找间接索引快
    if(inode->entry.indirect!=0){
        //有间接索引块
        Block* indirectBlock = cache->acquire(inode->entry.indirect);
        //获取数组
        u32* addrs = get_addrs(indirectBlock);
        for(usize i = 0;i<INODE_NUM_INDIRECT;i++){
            if (addrs[i]){
                //有分配块
                cache->free(ctx,addrs[i]);
            }
        }
        cache->release(indirectBlock);
        cache->free(ctx,inode->entry.indirect);
        inode->entry.indirect = 0;
    }
    //再处理直接索引块
    for(usize i =0;i<INODE_NUM_DIRECT;i++){
        if(inode->entry.addrs[i]){
            cache->free(ctx,inode->entry.addrs[i]);
            inode->entry.addrs[i]=0;//清空块
        }
    }
    inode->entry.num_bytes =0;
    inode_sync(ctx,inode,true);//及时写回
}

// see `inode.h`.
static Inode* inode_share(Inode* inode) {
    // TODO
    increment_rc(&inode->rc);
    return inode;
}

// see `inode.h`.
static void inode_put(OpContext* ctx, Inode* inode) {
    // TODO
    // 问题：如何保证只有一个线程进行删除？可以在这里等待get的操作完成，即post了？
    unalertable_acquire_sleeplock(&inode->lock);
    decrement_rc(&inode->rc);
    if(inode->rc.count ==0&&inode->entry.num_links==0){
        inode->entry.type = INODE_INVALID;
        inode_clear(ctx,inode);
        inode_sync(ctx,inode,true);
        acquire_spinlock(&lock);
        _detach_from_list(&inode->node);
        release_spinlock(&lock);
        post_sem(&inode->lock);
        kfree(inode);
        return;
    }
    post_sem(&inode->lock);
}

/**
    @brief get which block is the offset of the inode in.

    e.g. `inode_map(ctx, my_inode, 1234, &modified)` will return the block_no
    of the block that contains the 1234th byte of the file
    represented by `my_inode`.

    If a block has not been allocated for that byte, `inode_map` will
    allocate a new block and update `my_inode`, at which time, `modified`
    will be set to true.

    HOWEVER, if `ctx == NULL`, `inode_map` will NOT try to allocate any new block,
    and when it finds that the block has not been allocated, it will return 0.
    
    @param[out] modified true if some new block is allocated and `inode`
    has been changed.

    @return usize the block number of that block, or 0 if `ctx == NULL` and
    the required block has not been allocated.

    @note the caller must hold the lock of `inode`.
 */
static usize inode_map(OpContext* ctx,
                       Inode* inode,
                       usize offset,
                       bool* modified) {
    // TODO
    ASSERT(inode != NULL);
    ASSERT(modified != NULL);
    if(offset <INODE_NUM_DIRECT){
        //现在直接块里面分配
        if(inode->entry.addrs[offset]){
            return inode->entry.addrs[offset];
        }
        else if(inode->entry.addrs[offset]==0&&ctx==NULL){
            return 0;
        }
        else if(ctx!=NULL&&inode->entry.addrs[offset]==0){
            //ctx不为NULL时分配
            *modified = true;
            inode->entry.addrs[offset] =cache->alloc(ctx);
            //inode被更改了，记录
            inode_sync(ctx,inode,true);
            return inode->entry.addrs[offset];
        }
    }

    //offset大于INODE_NUM_DIRECT
    offset -= INODE_NUM_DIRECT;
    if(inode->entry.indirect==0&&ctx==NULL){
        return 0;//没有ctx不能分配新块
    }
    else if(inode->entry.indirect==0&&ctx!=NULL){
        //分配新块
        inode->entry.indirect = cache->alloc(ctx);
        //inode被更改了，记录
        inode_sync(ctx,inode,true);
    }
    Block* indirectBlock = cache->acquire(inode->entry.indirect);
    u32* addrs = get_addrs(indirectBlock);
    if(addrs[offset]==0&&ctx==NULL) return 0;
    else if(addrs[offset]==0&&ctx!=NULL){
        *modified =1;
        addrs[offset] = cache->alloc(ctx);
        cache->sync(ctx, indirectBlock);
    }
    usize blockno = addrs[offset];
    cache->release(indirectBlock);
    return blockno;
}

// see `inode.h`.
static usize inode_read(Inode* inode, u8* dest, usize offset, usize count) {
    InodeEntry* entry = &inode->entry;
    //判断是终端，调用console_read
    if (inode->entry.type == INODE_DEVICE) {
        return console_read(inode, (char*)dest, count);
    }
    //如果count过长
    if (count + offset > entry->num_bytes)
        count = entry->num_bytes - offset;
    usize end = offset + count;
    ASSERT(offset <= entry->num_bytes);
    ASSERT(end <= entry->num_bytes);
    ASSERT(offset <= end);

    // TODO
    for(usize i  =offset;i<end;i=(i/BLOCK_SIZE +1)*BLOCK_SIZE){
        //块对齐
        bool modified =0;
        usize block_no =inode_map(NULL,inode,i/BLOCK_SIZE,&modified);
        Block* block = cache->acquire(block_no);
        //三种情况[offset,BLOCK_SIZE],[0,BLOCK_SIZE],[0,end]
        //可以发现总是取最短的
        usize len  =MIN(BLOCK_SIZE-i%BLOCK_SIZE,end-i);
        memcpy(dest,block->data +i%BLOCK_SIZE,len);
        dest+=len;
        cache->release(block);
    }
    return count;
}

// see `inode.h`.
static usize inode_write(OpContext* ctx,
                         Inode* inode,
                         u8* src,
                         usize offset,
                         usize count) {
    InodeEntry* entry = &inode->entry;
    usize end = offset + count;
     if (inode->entry.type == INODE_DEVICE) {
        return console_write(inode, (char*)src, count);
    }
    ASSERT(offset <= entry->num_bytes);
    ASSERT(end <= INODE_MAX_BYTES);
    ASSERT(offset <= end);
    // TODO
    if(entry->num_bytes<end){
        entry->num_bytes = end;
        //inode发生变化
        inode_sync(ctx,inode,true);
    }
    bool modified = false;
    u8* dest;
    //与read类似，遍历每一块
    for (usize i = 0, cnt = 0; i < count; i += cnt, src += cnt, offset += cnt) {
        usize block_no = inode_map(ctx, inode, offset / BLOCK_SIZE, &modified);
        Block* block = cache->acquire(block_no);
        if (i == 0) {
            cnt = MIN(BLOCK_SIZE - offset % (usize)BLOCK_SIZE, (count));
            dest = block->data + offset % (usize)BLOCK_SIZE;
        } else {
            cnt = MIN((usize)BLOCK_SIZE, count - i);
            dest = block->data;
        }
        memcpy(dest, src, cnt);
        cache->sync(ctx, block);
        cache->release(block);
    }
    if (inode->entry.num_bytes < end) {
        inode->entry.num_bytes = end;
        inode_sync(ctx, inode, TRUE);
    }
    return count;
}

// see `inode.h`.
static usize inode_lookup(Inode* inode, const char* name, usize* index) {
    InodeEntry* entry = &inode->entry;
    ASSERT(entry->type == INODE_DIRECTORY);
    //index 最后要更新！！否则insert的时候不知道插在哪儿。。
    // TODO
    DirEntry cur;
    //printk("want to find name:%s\n",name);
    for(usize i = 0;i<entry->num_bytes;i+=sizeof(DirEntry)) {
    //遍历目录文件下的所有文件，记住是inode_no和文件名
        inode_read(inode,(u8 *)&cur,i,sizeof(DirEntry));//不知道能不能行，反正先强制转换了(,总之应该能读到cur吧。。。。。
        // if(i==0)
        //   printk("cur.inode_no = %zu, cur.name = %s\n", cur.inode_no, cur.name);
        if(cur.inode_no&&(memcmp(cur.name,name,MAX(strlen(name),strlen(cur.name)))==0)){
            if(index) *index = i;
            return cur.inode_no;
        }
    }
    //看了下测试文件，要求是如果没找到则不更新inode.....
    //分成两个版本就行了
    return 0;
}

static usize lookup(Inode* inode, const char* name, usize* index) {
    InodeEntry* entry = &inode->entry;
    ASSERT(entry->type == INODE_DIRECTORY);
    //index 最后要更新！！否则insert的时候不知道插在哪儿。。
    // TODO
    DirEntry cur;
    //printk("want to find name:%s\n",name);
    for(usize i = 0;i<entry->num_bytes;i+=sizeof(DirEntry)) {
    //遍历目录文件下的所有文件，记住是inode_no和文件名
        inode_read(inode,(u8 *)&cur,i,sizeof(DirEntry));//不知道能不能行，反正先强制转换了(,总之应该能读到cur吧。。。。。
        // if(i==0)
        //   printk("cur.inode_no = %zu, cur.name = %s\n", cur.inode_no, cur.name);
        if(cur.inode_no&&(memcmp(cur.name,name,MAX(strlen(name),strlen(cur.name)))==0)){
            if(index) *index = i;
            return cur.inode_no;
        }
    }
    if(index)
        *index = entry->num_bytes;//更新为末尾，方便插入

    return 0;
}

// see `inode.h`.
static usize inode_insert(OpContext* ctx,
                          Inode* inode,
                          const char* name,
                          usize inode_no) {
    InodeEntry* entry = &inode->entry;
    ASSERT(entry->type == INODE_DIRECTORY);

    // TODO
    usize index=0;
    //先检查有没有，如果有了直接退出
    if(lookup(inode,name,&index))
        return -1;
    DirEntry new;
    new.inode_no = inode_no;
    strncpy(new.name,name,FILE_NAME_MAX_LENGTH);
    inode_write(ctx,inode,(u8 *)&new,index,sizeof(DirEntry));
    return 0;
}

// see `inode.h`.
static void inode_remove(OpContext* ctx, Inode* inode, usize index) {
    // TODO
    DirEntry now;
    inode_read(inode,(u8*)&now,index,sizeof(DirEntry));
    now.inode_no=0;
    inode_write(ctx,inode,(u8*)&now,index,sizeof(DirEntry));
}

InodeTree inodes = {
    .alloc = inode_alloc,
    .lock = inode_lock,
    .unlock = inode_unlock,
    .sync = inode_sync,
    .get = inode_get,
    .clear = inode_clear,
    .share = inode_share,
    .put = inode_put,
    .read = inode_read,
    .write = inode_write,
    .lookup = inode_lookup,
    .insert = inode_insert,
    .remove = inode_remove,
};

/**
    @brief read the next path element from `path` into `name`.
    
    @param[out] name next path element.

    @return const char* a pointer offseted in `path`, without leading `/`. If no
    name to remove, return NULL.

    @example 
    skipelem("a/bb/c", name) = "bb/c", setting name = "a",
    skipelem("///a//bb", name) = "bb", setting name = "a",
    skipelem("a", name) = "", setting name = "a",
    skipelem("", name) = skipelem("////", name) = NULL, not setting name.
 */
static const char* skipelem(const char* path, char* name) {
    const char* s;
    int len;

    while (*path == '/')
        path++;
    if (*path == 0)
        return 0;
    s = path;
    while (*path != '/' && *path != 0)
        path++;
    len = path - s;
    if (len >= FILE_NAME_MAX_LENGTH)
        memmove(name, s, FILE_NAME_MAX_LENGTH);
    else {
        memmove(name, s, len);
        name[len] = 0;
    }
    while (*path == '/')
        path++;
    return path;
}

/**
    @brief look up and return the inode for `path`.

    If `nameiparent`, return the inode for the parent and copy the final
    path element into `name`.
    
    @param path a relative or absolute path. If `path` is relative, it is
    relative to the current working directory of the process.

    @param[out] name the final path element if `nameiparent` is true.

    @return Inode* the inode for `path` (or its parent if `nameiparent` is true), 
    or NULL if such inode does not exist.

    @example
    namex("/a/b", false, name) = inode of b,
    namex("/a/b", true, name) = inode of a, setting name = "b",
    namex("/", true, name) = NULL (because "/" has no parent!)
 */
static Inode* namex(const char* path,
                    bool nameiparent,
                    char* name,
                    OpContext* ctx) {
    /* (Final) TODO BEGIN */
    Inode *cur_inode;
    if(*path == '/'){
     //根目录
        cur_inode=inode_get(ROOT_INODE_NO);
    }
    else{
        //当前目录
        cur_inode=inode_share(thisproc()->cwd);
    }

    //然后跟着path一级一级往下找，使用skipelem
    path = skipelem(path,name);
    Inode * next_inode;
    usize next_inode_no;
    while(path!=NULL){
        inode_lock(cur_inode);
        //判断下找没找到
        if (cur_inode->entry.type!=INODE_DIRECTORY){
            inode_unlock(cur_inode);
            inode_put(ctx,cur_inode);
            return NULL;
        }
        if(nameiparent&&*path=='\0'){
            //如果需要返回parent，则此时next将会是最后一级，则直接返回cur_inode
            //name应该在skipelem更新好了？应该没错吧....
            inode_unlock(cur_inode);
            return cur_inode;
        }
        
        //找下一级
        next_inode_no = inode_lookup(cur_inode,name,0);
        next_inode = inode_get(next_inode_no);
        if(next_inode==NULL){
            //没找到，说明不存在这样的文件，先unlock然后put一下
            inode_unlock(cur_inode);
            inode_put(ctx,cur_inode);
            return NULL;
        }
        //如果到这里，说明next更新了，put一下cur，然后更新cur
        inode_unlock(cur_inode);
        inode_put(ctx,cur_inode);
        cur_inode = next_inode;
        path = skipelem(path,name);

    }
    //从这里出来，有两种可能
    //1.nameiparent 为false，直接返回cur_inode即可
    //2.nameiparent 为true，但它应该在while内返回啊？说明path不为0，有错
    if(nameiparent){
        inode_put(ctx,cur_inode);
        return NULL;
    }

    /* (Final) TODO END */
    return cur_inode;
}

Inode* namei(const char* path, OpContext* ctx) {
    char name[FILE_NAME_MAX_LENGTH];
    return namex(path, false, name, ctx);
}

Inode* nameiparent(const char* path, char* name, OpContext* ctx) {
    return namex(path, true, name, ctx);
}

/**
    @brief get the stat information of `ip` into `st`.
    
    @note the caller must hold the lock of `ip`.
 */
void stati(Inode* ip, struct stat* st) {
    st->st_dev = 1;
    st->st_ino = ip->inode_no;
    st->st_nlink = ip->entry.num_links;
    st->st_size = ip->entry.num_bytes;
    switch (ip->entry.type) {
        case INODE_REGULAR:
            st->st_mode = S_IFREG;
            break;
        case INODE_DIRECTORY:
            st->st_mode = S_IFDIR;
            break;
        case INODE_DEVICE:
            st->st_mode = 0;
            break;
        default:
            PANIC();
    }
}