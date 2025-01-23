#include <aarch64/intrinsic.h>
#include <kernel/cpu.h>
#include <kernel/printk.h>
#include <kernel/sched.h>
#include <test/test.h>
#include <common/buf.h>
#include <driver/virtio.h>
#include <kernel/proc.h>
#include <kernel/sched.h>

volatile bool panic_flag;
extern char icode[];
extern char eicode[];
NO_RETURN void idle_entry()
{
    set_cpu_on();
    while (1) {
        yield();
        if (panic_flag)
            break;
        arch_with_trap
        {
            arch_wfi();
        }
    }
    set_cpu_off();
    arch_stop_cpu();
}

NO_RETURN void kernel_entry()
{
    init_filesystem();

    printk("Hello world! (Core %lld)\n", cpuid());
    //kalloc_test();
    // proc_test();
    // vm_test();
    // user_proc_test();
    //io_test();
    // pgfault_first_test();
    // pgfault_second_test();
    /* LAB 4 TODO 3 BEGIN */
    // Buf mbr_buf;
    // mbr_buf.block_no = 0;        // 设置读取MBR的块号
    // mbr_buf.flags = B_VALID;     // 标记为有效，不需要写入

    // 调用 virtio_blk_rw 读取MBR数据
    // if (virtio_blk_rw(&mbr_buf) < 0) {
    //     printk("Failed to read MBR\n");
    //     while (1) yield();
    // }

    // // 等待 I/O 操作完成
    // //wait_sem(&mbr_buf.sem);

    // // 检查读取是否成功
    // if (!(mbr_buf.flags & B_VALID)) {
    //     printk("Failed to validate MBR\n");
    //     while (1) yield();
    // }

    // // 解析MBR，找到第二分区
    // //u32 *partition_table = (u32 *)(mbr_buf.data + 446);
    // u32 second_partition_lba = *(u32 *)(mbr_buf.data + 446 + 16 + 8);   // 第二分区起始 LBA
    // u32 second_partition_size = *(u32 *)(mbr_buf.data + 446 + 16 + 12); // 第二分区大小

    // printk("Second partition: LBA = %u, Size = %u\n", second_partition_lba, second_partition_size);
    /* LAB 4 TODO 3 END */

    /**
     * (Final) TODO BEGIN 
     * 
     * Map init.S to user space and trap_return to run icode.
     */
        auto p = create_proc();
        for (u64 q = PAGE_BASE((u64)(icode)); q <= (u64)eicode; q += PAGE_SIZE)
        {
            *get_pte(&p->pgdir, 0x0 + q - (u64)icode, true) = K2P(q) | PTE_USER_DATA | PTE_RO;
        }
        p->ucontext->x[0] = 0;
        p->ucontext->elr = 0x0 + (u64)icode - (PAGE_BASE((u64)icode));
        p->cwd=inodes.share(inodes.root);
        set_parent_to_this(p);
        start_proc(p, trap_return, 0);
    while (1)
    {
        if(wait(NULL)==false)
            continue;
        // yield();
        // arch_with_trap
        // {
        //     arch_wfi();
        // }
    }
    /* (Final) TODO END */
}

NO_INLINE NO_RETURN void _panic(const char *file, int line)
{
    printk("=====%s:%d PANIC%lld!=====\n", file, line, cpuid());
    panic_flag = true;
    set_cpu_off();
    for (int i = 0; i < NCPU; i++) {
        if (cpus[i].online)
            i--;
    }
    printk("Kernel PANIC invoked at %s:%d. Stopped.\n", file, line);
    arch_stop_cpu();
}