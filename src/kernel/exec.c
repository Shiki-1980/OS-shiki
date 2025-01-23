#include <elf.h>
#include <common/string.h>
#include <common/defines.h>
#include <kernel/console.h>
#include <kernel/proc.h>
#include <kernel/sched.h>
#include <kernel/syscall.h>
#include <kernel/pt.h>
#include <kernel/mem.h>
#include <kernel/paging.h>
#include <aarch64/trap.h>
#include <fs/file.h>
#include <fs/inode.h>
#include <kernel/printk.h>


extern int fdalloc(struct file *f);


int execve(const char *path, char *const argv[], char *const envp[]) {
	// TODO
	Proc* curproc=thisproc();
	//printk("exec %s\n",path);
	struct pgdir oldpigdir=curproc->pgdir;
	struct pgdir* pgdir=kalloc(sizeof(struct pgdir));
	init_pgdir(pgdir);
	Inode* ip=NULL;

	OpContext ctx;
	Elf64_Ehdr elf;
	uint64_t off;
	Elf64_Phdr ph;

	// ====================== 加载 ELF 文件 ======================
	bcache.begin_op(&ctx);
	ip=namei(path,&ctx);
	if (ip==NULL){
		bcache.end_op(&ctx);
		goto bad;
	}
	inodes.lock(ip);
	
	if (inodes.read(ip,(u8*)&elf,0,sizeof(elf))!=sizeof(elf)){
		goto bad;
	}

	//检查魔数
	if (!(elf.e_ident[EI_MAG0] == ELFMAG0 && 
		  elf.e_ident[EI_MAG1] == ELFMAG1 &&
		  elf.e_ident[EI_MAG2] == ELFMAG2 && 
		  elf.e_ident[EI_MAG3] == ELFMAG3)) {
        goto bad;
    }


	 // ====================== 加载程序段到内存 ======================
	int i=0;
	
	curproc->pgdir=*pgdir;
	u64 sz=0;

	bool f = true;
	// printk("elf.e_phnum:%d\n",elf.e_phnum);
	for(i=0,off=elf.e_phoff;i<elf.e_phnum;i++,off+=sizeof(ph)) {
		if ((inodes.read(ip,(u8*)&ph,off,sizeof(ph)))!=sizeof(ph)){
			goto bad;
		}
		if (ph.p_type!=PT_LOAD){
			continue;
		}
		if (ph.p_memsz < ph.p_filesz) {
            goto bad;
        }
        if (ph.p_vaddr + ph.p_memsz < ph.p_vaddr) {
            goto bad;
        }
		if(f){
			f = false;
			sz = ph.p_vaddr;
		}
		if ((sz=uvmalloc(pgdir,sz,ph.p_vaddr + ph.p_memsz))==0){
			goto bad;
		}
		attach_pgdir(pgdir);
    	arch_tlbi_vmalle1is();
		if (inodes.read(ip, (u8 *)ph.p_vaddr, ph.p_offset, ph.p_filesz)!=ph.p_filesz){
			goto bad;
		}
	}
	inodes.unlock(ip);
	inodes.put(&ctx,ip);
	bcache.end_op(&ctx);
	ip=NULL;
    // ====================== 准备用户栈 ======================
	// 处理参数和环境变量，设置新的栈指针
	attach_pgdir(&oldpigdir);
	arch_tlbi_vmalle1is();
	char *sp=(char*)USERTOP;
	int argc=0,envc=0;
	usize len;
	if (argv) {
        for (; argc < MAXARG && argv[argc]; argc++) {
            len = strlen(argv[argc]);
            sp -= len + 1;
            copyout(pgdir, sp, argv[argc], len + 1);// \0
        }
    }
    if (envp) {
        for (; envc < MAXARG && envp[envc]; envc++) {
            len = strlen(envp[envc]);
            sp -= len + 1;
            copyout(pgdir, sp, envp[envc], len + 1);
        }
    }
	//arcv有n+1个，envp有m+1个，还有一个argc,4B，应该是m+n+3，额，试了下，pagefault
	//又考虑到对齐的事情，应该都是向下对齐的....感觉不会重叠啊，最后试了下至少+4才行,难道是返回地址？
	void *newsp = (void *)(((usize)sp - (envc + argc + 4) * 8 ) & ~0xF);
    copyout(pgdir, newsp, NULL, (void *)sp - newsp);
    attach_pgdir(pgdir);
    arch_tlbi_vmalle1is();
    uint64_t *newargv = newsp + 8;
    uint64_t *newenvp = (void *)newargv + 8 * (argc + 1);
	//准备用户栈
	for (int i=envc-1;i>=0;i--){
		newenvp[i]=(u64)sp;
		sp += strlen((char*)newenvp[i])+1;
	}
	for (int i=argc-1;i>=0;i--){
		newargv[i]=(u64)sp;
		sp += strlen((char *)newargv[i])+1;
	}

	// ====================== 更新进程上下文 ======================
	*(usize*)(newsp)=argc;

	sp=newsp;
	u64 user_base = 0;
	user_base=(USERTOP - (usize)sp+8*PAGE_SIZE-1) & ~0x7F;
	copyout(pgdir,(void *)(USERTOP - user_base),0, user_base - (USERTOP - (usize)sp));
	
	curproc->pgdir=*pgdir;
	curproc->ucontext->elr=elf.e_entry;
	curproc->ucontext->sp=(u64)sp;

	free_pgdir(&oldpigdir);
	//printk("加载成功！");
	return 0;

bad:
	if (pgdir) free_pgdir(pgdir);
	if (ip){
		inodes.unlock(ip);
		inodes.put(&ctx,ip);
		bcache.end_op(&ctx);
	}
	thisproc()->pgdir=oldpigdir;
	return -1;
}