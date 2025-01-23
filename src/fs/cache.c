#include <common/bitmap.h>
#include <common/string.h>
#include <fs/cache.h>
#include <kernel/mem.h>
#include <kernel/printk.h>
#include <kernel/proc.h>
/**
    @brief the private reference to the super block.

    @note we need these two variables because we allow the caller to
            specify the block device and super block to use.
            Correspondingly, you should NEVER use global instance of
            them, e.g. `get_super_block`, `block_device`

    @see init_bcache
 */
static const SuperBlock *sblock;

/**
    @brief the reference to the underlying block device.
 */
static const BlockDevice *device; 

/**
    @brief global lock for block cache.

    Use it to protect anything you need.

    e.g. the list of allocated blocks, etc.
 */
static SpinLock lock;

/**
 * 
   @brief global lock for bitmap.
 
 **/
static SpinLock bitlock;

/**
    @brief the list of all allocated in-memory block.

    We use a linked list to manage all allocated cached blocks.

    You can implement your own data structure if you like better performance.

    @see Block
 */
static ListNode head;

static usize cached_block_count = 0;  // 初始化为0，表示缓存中没有块

static LogHeader header; // in-memory copy of log header block.

/**
    @brief a struct to maintain other logging states.
    
    You may wonder where we store some states, e.g.
    
    * how many atomic operations are running?
    * are we checkpointing?
    * how to notify `end_op` that a checkpoint is done?

    Put them here!

    @see cache_begin_op, cache_end_op, cache_sync
 */

static BitmapCell block_bitmap[BITMAP_TO_NUM_CELLS(EVICTION_THRESHOLD)];

static void init_block_bitmap() {
    usize num_cells = BITMAP_TO_NUM_CELLS(EVICTION_THRESHOLD);
    for (usize i = 0; i < num_cells; i++) {
        block_bitmap[i] = 0;  // Set all bits in this cell to 0
    }
}

struct {
    SpinLock lock;
    Semaphore sem;
    int start;
    int size;
    int outstanding; // how many FS sys calls are executing.
    int committing;  // in commit(), please wait.
    int dev;
    //LogHeader lh;           // 内存中的日志头
} log;

void insert_to_head(ListNode *head, ListNode *new_node) {
    // 新节点插入到链表的头部（在head节点之后）
    new_node->next = head->next;
    new_node->prev = head;
    head->next->prev = new_node;
    head->next = new_node;
}


// read the content from disk.
static INLINE void device_read(Block *block) {
    device->read(block->block_no, block->data);
}

// write the content back to disk.
static INLINE void device_write(Block *block) {
    device->write(block->block_no, block->data);
}

// read log header from disk.
static INLINE void read_header() {
    device->read(sblock->log_start, (u8 *)&header);
}

// write log header back to disk.
static INLINE void write_header() {
    device->write(sblock->log_start, (u8 *)&header);
}

// initialize a block struct.
static void init_block(Block *block) {
    block->block_no = 0;
    init_list_node(&block->node);
    block->acquired = false;
    block->pinned = false;
    init_sleeplock(&block->lock);
    block->valid = false;
    memset(block->data, 0, sizeof(block->data));
}


void remove_node(ListNode *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

// see `cache.h`.
static usize get_num_cached_blocks() {
    // TODO
    return cached_block_count;
}

// see `cache.h`.
static Block *cache_acquire(usize block_no) {
    // TODO

    acquire_spinlock(&lock);
    // 检查块是否已在缓存中
    Block *block = NULL;
    ListNode *node = head.next;
    while (node != &head) {
        Block *cached_block = container_of(node, Block, node);
        if (cached_block->block_no == block_no) {
            block = cached_block;
            break;
        }
        node = node->next;
    }

    if (block) {
        // 如果块在缓存中，标记为已获取并返回
        block->acquired = true;
        release_spinlock(&lock);
        bool flag = acquire_sleeplock(&block->lock);
        if(!flag){
            printk("wait_sem异常");
            return 0;
            }
        acquire_spinlock(&lock);
        _detach_from_list(&block->node);
        insert_to_head(&head,&block->node);
        release_spinlock(&lock);
        return block;
    }

    // 检查缓存数量是否超过阈值，超过则驱逐一块
    if (cached_block_count >= EVICTION_THRESHOLD) {
        node = head.prev;
        ListNode * pre;
        while (node != &head) {
            Block *evict_block = container_of(node,Block,node);
            pre = node->prev;
            if (!evict_block->acquired && !evict_block->pinned) {
                // 如果找到未被获取且未固定的块，驱逐
                _detach_from_list(node);//要保存node的prev，否则丢失了！
                cached_block_count--; 
                kfree(evict_block);
                break;
            }
            node = pre;
        }
    }
    block = kalloc(sizeof(Block));
    init_block(block);

    bool flag = acquire_sleeplock(&block->lock);
    if(!flag){
        printk("wait_sem异常");
        return 0;
    }
    block->block_no = block_no;
    block->valid = true;
    block->acquired = true;
    cached_block_count++;
    // 将块加入缓存链表并增加计数
    release_spinlock(&lock);
    device_read(block);
    acquire_spinlock(&lock);
    insert_to_head(&head, &block->node);
    release_spinlock(&lock);

    return block;

}

// see `cache.h`.
static void cache_release(Block *block) {
    // TODO
    acquire_spinlock(&lock);
    block->acquired = false;
    //不能在这里释放掉锁！这次release
    release_sleeplock(&block->lock);
    release_spinlock(&lock);
}

static void cache_sync(OpContext *ctx, Block *block) ;//使用前要先声明
static void init_log(const SuperBlock* sb) {
    init_spinlock(&log.lock);
    log.start = sb->log_start;      // 日志区域的起始块
    log.size = sb->num_log_blocks;  // 日志区域的块数
    log.outstanding = 0;            // 当前正在执行的文件系统调用数量
    log.committing = false;         // 是否正在执行提交操作
    init_sem(&log.sem,0);
    header.num_blocks =0;
    // 读取log header
    read_header();//header中有了log_start，接着要恢复所有的log
    for(usize i=1;i<=header.num_blocks;i++){
        Block* block_in_super = cache_acquire(sb->log_start+i);
        Block* recovery_block = cache_acquire(header.block_no[i-1]);
        for(int j =0;j<BLOCK_SIZE;j++){
            recovery_block->data[j] = block_in_super->data[j];
        }
        //每次写都要sync，写完后chache中的block释放
        cache_sync(NULL,recovery_block);
        cache_release(block_in_super);
        cache_release(recovery_block);
    }
    //恢复完cache后清空header
    header.num_blocks=0;
    memset(header.block_no,0,LOG_MAX_SIZE);
    write_header();
    //将空的log_header写回superblock
    //log.dev = device->dev_id;       // 设备ID，可以根据需要进行修改，假设有设备ID
}
// see `cache.h`.
void init_bcache(const SuperBlock *_sblock, const BlockDevice *_device){
    // 设置传入的全局块缓存引用
    sblock = _sblock;
    device = _device;
    //TODO
    init_spinlock(&lock);
    init_spinlock(&bitlock);
    init_list_node(&head);
    cached_block_count = 0;
    init_log(sblock);
    init_block_bitmap();
    return ;
}

// see `cache.h`.
static void cache_begin_op(OpContext *ctx) {

    ctx->rm = OP_MAX_NUM_BLOCKS;  // 已经操作的操作数，初始化为0，最大值为OP_MAX_NUM_BLOCKS

    acquire_spinlock(&log.lock);

    // 等待直到有足够的空间进行新操作
    while (1) {
        if (log.committing||header.num_blocks+(log.outstanding +1)*OP_MAX_NUM_BLOCKS >LOG_MAX_SIZE) {//mit xv6，写完后待我细细揣摩一下OvO
            // 如果正在进行checkpoint，等待,记得释放日志锁
            release_spinlock(&log.lock);
            bool flag = wait_sem(&log.sem);
            if(!flag){
                printk("wait_sem异常");
                return;
            }
            acquire_spinlock(&log.lock);//被唤醒后获取锁
        } 
        else {
            // 有足够的空间，可以开始新操作
            log.outstanding += 1;
            break;
        }
    }

    release_spinlock(&log.lock);
}

// see `cache.h`.
static void cache_sync(OpContext *ctx, Block *block) {
    if (ctx == NULL) {
        //没有上下文，直接写，
        device_write(block);
        return;
    }
    acquire_spinlock(&log.lock);
    block->pinned = true;
    //准备将块写到日志中，先在日志中找一下
    for(usize i = 0;i<header.num_blocks;i++){
        if(block->block_no == header.block_no[i]){
            //在日志中找到，不用再加了
            release_spinlock(&log.lock);
            return;
        }
    }
    //否则写入header,还要在判断一下，如果超过10次或者日志满了，报错
    if(ctx->rm<=0){
        //printk("操作数超过上限\n");
        PANIC();
    }
    if(header.num_blocks >=LOG_MAX_SIZE){
        //printk("日志已满\n");
        PANIC();
    }
    header.block_no[header.num_blocks++] = block->block_no;
    ctx->rm--;
    release_spinlock(&log.lock);

}

// see `cache.h`.
static void cache_end_op(OpContext *ctx) {
    acquire_spinlock(&log.lock);

    log.outstanding -= 1;
    if (log.committing){
        printk("cache_end_op: 日志正在提交");
        PANIC();
    }
    if(log.outstanding>0){
        //还有其他的操作未完成
        post_sem(&log.sem);//告知sem已有一个完成
        _lock_sem(&log.sem);
        release_spinlock(&log.lock);//一定要释放锁！
        if(!_wait_sem(&log.sem,true))
            PANIC();
        return;
    }
    log.committing = true;
    //准备提交，将header中的内容写到superblock中：
    release_spinlock(&log.lock);
    for(usize i=1;i<=header.num_blocks;i++){
        Block* block_in_super = cache_acquire(sblock->log_start+i);
        Block* recovery_block = cache_acquire(header.block_no[i-1]);
        for(int j =0;j<BLOCK_SIZE;j++){
            block_in_super->data[j]= recovery_block->data[j];
        }
        cache_sync(NULL,block_in_super);
        cache_release(block_in_super);
        cache_release(recovery_block);
    }
    //release_spinlock(&log.lock);
    write_header();//将header写到super中，
    acquire_spinlock(&log.lock);    

    //准备将块写到设备中
    for(usize i =0;i<header.num_blocks;i++){
        Block* block = cache_acquire(header.block_no[i]);
        cache_sync(NULL,block);//直接写到device中
        //写完了，不用固定这个块了
        block->pinned=false;
        cache_release(block);
    }
    //全写入设备后，清空日志并写入superblock
    header.num_blocks= 0;
    memset(header.block_no,0,LOG_MAX_SIZE);
    write_header();
    log.committing = false;
    post_all_sem(&log.sem);//都提交了，全部post了
    release_spinlock(&log.lock);
    return (void) ctx;
}

// see `cache.h`.
static usize cache_alloc(OpContext* ctx) {
    // TODO
    acquire_spinlock(&bitlock);
    for (usize block_count=0;block_count<sblock->num_blocks;block_count += BLOCK_SIZE * 8){
        //每个位图块有BLOCK_SIZE个字节，每个字节有8个bit
        //可以通过这个直接获取位图块
        Block* bitmap_block = cache_acquire(sblock->bitmap_start+block_count/(BLOCK_SIZE * 8));
        for (usize idx=0;idx<BLOCK_SIZE &&  block_count + idx*8       < sblock->num_blocks;idx++){
            //遍历位图块的每一个字节
            int bits=bitmap_block->data[idx];
            for (usize offset=0; offset <8 && block_count + idx*8+offset< sblock->num_blocks;offset++){
                //遍历每个字节的8个bit
                if (!((bits>> offset) & 1)){//左移offset位，如果该位为1则结果非0，表示已被占用
                    bitmap_block->data[idx]|=(1<<offset);//更改bitmap
                    //写入device中，注意这肯定在事务中，要消耗ctx
                    cache_sync(ctx,bitmap_block);
                    cache_release(bitmap_block);
                    Block* target_block=cache_acquire(block_count+idx*8+offset);
                    memset(target_block->data,0,BLOCK_SIZE);
                    cache_sync(ctx,target_block);
                    cache_release(target_block);
                    release_spinlock(&bitlock);
                    return  block_count+idx*8+offset;
                }
            }
        }
        cache_release(bitmap_block);
    }
    release_spinlock(&bitlock);
    PANIC();
}


// see `cache.h`.
static void cache_free(OpContext *ctx, usize block_no) {
    // Acquire lock to ensure thread safety
    acquire_spinlock(&bitlock);
    // 直接整除块大小*8得到位图块的序号
    Block* bitmap_block=cache_acquire(sblock->bitmap_start+block_no/(BLOCK_SIZE * 8));
    int idx, offset;
    // 得到在块中的序号以及偏移
    usize bit_block_no = block_no %(BLOCK_SIZE * 8);//得到在位图块中的正确序号
    idx = bit_block_no / 8 ;//得到在位图块中的序号,每一个字节有8个bit!
    offset = bit_block_no % 8 ;//得到在位图块中的偏移量
    bitmap_block->data[idx]&= ~BIT(offset);
    cache_sync(ctx,bitmap_block);
    cache_release(bitmap_block);
    release_spinlock(&bitlock);
}

BlockCache bcache = {
    .get_num_cached_blocks = get_num_cached_blocks,
    .acquire = cache_acquire,
    .release = cache_release,
    .begin_op = cache_begin_op,
    .sync = cache_sync,
    .end_op = cache_end_op,
    .alloc = cache_alloc,
    .free = cache_free,
};