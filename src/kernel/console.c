#include <kernel/console.h>
#include <aarch64/intrinsic.h>
#include <kernel/sched.h>
#include <driver/uart.h>
#include <driver/interrupt.h>
#include <common/string.h>


#define BACKSPACE 0x100


struct console cons;
void console_init()
{
    /* (Final) TODO BEGIN */
    init_spinlock(&cons.lock);
    init_sem(&cons.sem,0);
    /* (Final) TODO END */
}

static void consputc(int c)
{
  if(c == BACKSPACE){
    // if the user typed backspace, overwrite with a space.
    uart_put_char('\b'); uart_put_char(' '); uart_put_char('\b');
  } else {
    uart_put_char(c);
  }
}


/**
 * console_write - write to uart from the console buf.
 * @ip: the pointer to the inode
 * @buf: the buf
 * @n: number of bytes to write
 */
isize console_write(Inode *ip, char *buf, isize n)
{
    /* (Final) TODO BEGIN */
    inodes.unlock(ip);
    acquire_spinlock(&(cons.lock));
    for(isize i =0;i<n;i++){
        consputc(buf[i]);
    }
    release_spinlock(&(cons.lock));
    inodes.lock(ip);
    return n;
    /* (Final) TODO END */
}

/**
 * console_read - read to the destination from the buf
 * @ip: the pointer to the inode
 * @dst: the destination
 * @n: number of bytes to read
 */
isize console_read(Inode *ip, char *dst, isize n)
{
    /* (Final) TODO BEGIN */
    inodes.unlock(ip);
    acquire_spinlock(&(cons.lock));
    int target = n;
    while(n>0){
        //wait until put
        while(cons.read_idx == cons.write_idx){
            if(thisproc()->killed){
                release_spinlock(&(cons.lock));
                inodes.lock(ip);
                return -1;
            }
            release_spinlock(&cons.lock);
            unalertable_wait_sem(&cons.sem);
        }
        int c = cons.buf[cons.read_idx %IBUF_SIZE];
        cons.read_idx ++;
        if(c == C('D')){
            if(n <target){
                cons.read_idx --;
            }
            break;
        }
        *dst = c;
        dst++;
        --n;;
        if(c=='\n')
            break;
    }
    
    release_spinlock(&(cons.lock));
    inodes.lock(ip);
    return target - n;
    /* (Final) TODO END */
}


void console_intr(char c)
{
    /* (Final) TODO BEGIN */

    acquire_spinlock(&cons.lock);
    switch(c){
        case C('U'): //Ctrl + U ,kill line
            while(
                cons.edit_idx != cons.write_idx &&
                cons.buf[(cons.edit_idx-1)% IBUF_SIZE]!= '\n'
            ){
                cons.edit_idx --;
                consputc(BACKSPACE);
            }
            break;
        case C('H')://Backspace
        case '\x7f': //Delete key
            if(cons.edit_idx !=cons.write_idx){
                cons.edit_idx --;
                consputc(BACKSPACE);
            }
            break;
        case C('C'): 
                if (thisproc()->pid > 2) {
                    ASSERT(kill(thisproc()->pid) == 0);
                }
                uart_put_char('^');uart_put_char('C');uart_put_char('\n');uart_put_char('$');
                memset(cons.buf, 0, IBUF_SIZE);
                cons.edit_idx = cons.write_idx =cons.read_idx = 0;
                break;
        case C('X'):
            procdump();
            break;
        default:
            if( c!=0  && cons.edit_idx - cons.read_idx < IBUF_SIZE){
                c = (c=='\r') ? '\n' : c;
                consputc(c);

                cons.buf[cons.edit_idx++ % IBUF_SIZE] = c;

                if(c == '\n' || c== C('D')|| cons.edit_idx -cons.read_idx == IBUF_SIZE){
                    cons.write_idx = cons.edit_idx;
                    post_all_sem(&cons.sem);
                }
               
            }
    }
    release_spinlock(&cons.lock);
    /* (Final) TODO END */
}

