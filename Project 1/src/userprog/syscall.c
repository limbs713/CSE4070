#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "devices/shutdown.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"

static void syscall_handler(struct intr_frame *);

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler(struct intr_frame *f UNUSED)
{
  uint32_t *esp = f->esp;
  switch ((uint32_t)*esp)
  {
  case SYS_HALT:
    halt();
    break;
  case SYS_EXIT:
    check_user_addr((uint8_t *)esp, 1);
    exit(*(int *)((uint8_t *)esp + 4));
    break;
  case SYS_EXEC:
    check_user_addr((uint8_t *)esp, 1);
    f->eax = exec(*(char **)((uint8_t *)esp + 4));
    break;
  case SYS_WAIT:
    check_user_addr((uint8_t *)esp, 1);
    f->eax = wait(*(tid_t *)((uint8_t *)esp + 4));
    break;
  case SYS_READ:
    check_user_addr((uint8_t *)esp, 3);
    f->eax = read(*(int *)((uint8_t *)esp + 4),
                  *(void **)((uint8_t *)esp + 8),
                  *(unsigned *)((uint8_t *)esp + 12));
    break;
  case SYS_WRITE:
    check_user_addr((uint8_t *)esp, 3);
    f->eax = write(*(int *)((uint8_t *)esp + 4),
                   *(const void **)((uint8_t *)esp + 8),
                   *(unsigned *)((uint8_t *)esp + 12));
    break;
  case SYS_FIBONACCI:
    check_user_addr((uint8_t *)esp, 1);
    f->eax = fibonacci(*(int *)((uint8_t *)esp + 4));
    break;
  case SYS_MAXOFFOUR:
    check_user_addr((uint8_t *)esp, 4);
    f->eax = max_of_four_int(
        *(int *)((uint8_t *)esp + 4),
        *(int *)((uint8_t *)esp + 8),
        *(int *)((uint8_t *)esp + 12),
        *(int *)((uint8_t *)esp + 16));
    break;

  default:
    break;
  }
}

void check_user_addr(void *addr, int i)
{
  for (int j = 1; j <= i; j++)
  {
    if (!is_user_vaddr((uint8_t *)addr + 4 * j))
    {
      exit(-1);
    }
  }
}

void halt(void)
{
  shutdown_power_off();
}

void exit(int status)
{
  printf("%s: exit(%d)\n", thread_name(), status);
  thread_current()->exit_status = status;
  thread_exit();
}
tid_t exec(const char *cmd_line)
{
  return process_execute(cmd_line);
}

int wait(tid_t pid)
{
  return process_wait(pid);
}

int read(int fd, void *buffer, unsigned size)
{
  int byte_num = 0;
  if (fd == 0)
  {
    for (int i = 0; i < size; i++)
    {
      char c;
      if ((c = input_getc()) != '\0')
      {
        *(char *)buffer = c;
        buffer += 1;
        byte_num++;
      }
      else
        break;
    }
    *(char *)buffer = '\0';
    return byte_num;
  }
  else
  {
    return byte_num = -1;
  }
}

int write(int fd, const void *buffer, unsigned size)
{

  if (fd == 1)
  {
    putbuf(buffer, size);
    return size;
  }
  return 0;
}

int fibonacci(int n)
{
  int n1 = 0, n2 = 1, n3 = 0;

  if (n == 0)
    return 0;
  if (n == 1)
    return 1;

  for (int i = 2; i <= n; i++)
  {
    n1 = n2 + n3;
    n3 = n2;
    n2 = n1;
  }
  return n1;
}

int max_of_four_int(int a, int b, int c, int d)
{
  int max = a;
  if (max < b)
    max = b;
  if (max < c)
    max = c;
  if (max < d)
    max = d;

  return max;
}