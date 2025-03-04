#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"


//系统调用处理流程
// 用户态触发：通过 ecall 指令陷入内核，硬件自动保存寄存器到陷阱帧。
// 参数提取：调用 argint/argaddr/argstr 解析参数。
// 函数分发：根据系统调用号从 syscalls 表中获取处理函数并执行。
// 结果返回：将返回值写入陷阱帧的 a0 寄存器，用户态恢复执行。


// Fetch the uint64 at addr from the current process.
//这段代码是用于从当前进程的用户空间安全地读取一个 uint64 类型值的函数 fetchaddr，
//其核心逻辑是地址合法性检查和用户空间到内核空间的数据拷贝。
//从用户空间地址 addr 读取一个 uint64 值，存入内核空间指针 ip 指向的位置。

int
fetchaddr(uint64 addr, uint64 *ip)
{
  struct proc *p = myproc();
  if(addr >= p->sz || addr+sizeof(uint64) > p->sz)
    return -1;
  if(copyin(p->pagetable, (char *)ip, addr, sizeof(*ip)) != 0)
    return -1;
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.

//从用户空间虚拟地址 addr 处安全拷贝一个以空字符（\0）结尾的字符串到内核缓冲区 buf，并返回字符串长度（不包含终止符）。
int
fetchstr(uint64 addr, char *buf, int max)
{
  struct proc *p = myproc();// 获取当前进程控制块，包含页表、内存范围等信息。
  int err = copyinstr(p->pagetable, buf, addr, max);
  if(err < 0)
    return err;
  return strlen(buf);
}

//从当前进程的陷阱帧（Trapframe）中直接获取第 n 个系统调用参数（RISC-V架构下对应寄存器 a0-a5）。
static uint64
argraw(int n)
{
  struct proc *p = myproc();
  switch (n) {
  case 0:
    return p->trapframe->a0;
  case 1:
    return p->trapframe->a1;
  case 2:
    return p->trapframe->a2;
  case 3:
    return p->trapframe->a3;
  case 4:
    return p->trapframe->a4;
  case 5:
    return p->trapframe->a5;
  }
  panic("argraw");//若参数索引 n>5，触发内核崩溃（避免非法访问）
  return -1;
}

// Fetch the nth 32-bit system call argument.



//将第 n 个参数解释为整数（int类型）。
int
argint(int n, int *ip)
{
  *ip = argraw(n);
  return 0;
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
//将第 n 个参数解释为指针（uint64地址）。
int
argaddr(int n, uint64 *ip)
{
  *ip = argraw(n);
  return 0;
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (including nul), -1 if error.
int
argstr(int n, char *buf, int max)
{
  uint64 addr;
  if(argaddr(n, &addr) < 0)
    return -1;
  return fetchstr(addr, buf, max);// 安全拷贝字符串到内核缓冲区 
}

extern uint64 sys_chdir(void);
extern uint64 sys_close(void);
extern uint64 sys_dup(void);
extern uint64 sys_exec(void);
extern uint64 sys_exit(void);
extern uint64 sys_fork(void);
extern uint64 sys_fstat(void);
extern uint64 sys_getpid(void);
extern uint64 sys_kill(void);
extern uint64 sys_link(void);
extern uint64 sys_mkdir(void);
extern uint64 sys_mknod(void);
extern uint64 sys_open(void);
extern uint64 sys_pipe(void);
extern uint64 sys_read(void);
extern uint64 sys_sbrk(void);
extern uint64 sys_sleep(void);
extern uint64 sys_unlink(void);
extern uint64 sys_wait(void);
extern uint64 sys_write(void);
extern uint64 sys_uptime(void);
extern uint64 sys_trace(void);
//系统调用号到处理函数的映射表（如 SYS_fork 映射到 sys_fork）
static uint64 (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_trace]   sys_trace,  // New add
};

//syscall_names[]：
// 调试支持：将系统调用号转换为可读字符串（如 1->"fork"），用于日志、跟踪等场景。
// 扩展性：新增系统调用（如 trace）需同步更新名称表。
static char *syscall_names[23] = {"","fork","exit","wait","pipe","read","kill",
                                "exec","fstat","chdir","dup","getpid","sbrk","sleep",
                                "uptime","open","write","mknod","unlink","link","mkdir",
                                "close","trace"};  // New add
// void
// syscall(void)
// {
//   int num;
//   struct proc *p = myproc();

//   num = p->trapframe->a7;
//   if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
//     p->trapframe->a0 = syscalls[num]();
//   } else {
//     printf("%d %s: unknown sys call %d\n",
//             p->pid, p->name, num);
//     p->trapframe->a0 = -1;
//   }
// }



void
syscall(void)
{
  int num;
  struct proc *p = myproc();
  //char* syscall_name; 
 
  num = p->trapframe->a7;//系统调用号通过寄存器 a7
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) //NELEM(syscalls) 表示系统调用表长度
  {
    p->trapframe->a0 = syscalls[num]();//syscalls[num]() 执行对应的系统调用处理函数（如 sys_write）。
    // p->mask 为字符串，长度与系统调用数量一致（如 "000...1..."）。
    // mask[num] == '1' 表示需要记录第 num 号系统调用的执行结果。
    if (strlen(p->mask) > 0 && p->mask[num] == '1'){            // NEW ADD
        printf("%d: syscall %s -> %d\n", p->pid, syscall_names[num],p->trapframe->a0);
        //格式为 [PID]: syscall [名称] -> [返回值]（如 15: syscall write -> 512）。
    }
  } else {
    printf("%d %s: unknown sys call %d\n",
            p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}



//关键数据结构简述
//系统调用表 syscalls[]，映射规则：数组下标对应系统调用号（如 syscalls[16] = sys_write）
//系统调用名称表 syscall_names[]，调试支持：将系统调用号转换为可读名称（如 16 -> "write"）。
//进程控制块 struct proc
//  struct trapframe *trapframe; // 陷阱帧指针  
//  char name[16];               // 进程名称  
//  char mask[MAX_SYSCALLS];     // 调试掩码（新增）  
// //