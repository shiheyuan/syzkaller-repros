// general protection fault in copy_verifier_state
// https://syzkaller.appspot.com/bug?id=23a08526dadb017ac89247aa3d10a3012cd6e13c
// status:fixed
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

__attribute__((noreturn)) static void doexit(int status)
{
  volatile unsigned i;
  syscall(__NR_exit_group, status);
  for (i = 0;; i++) {
  }
}
#include <stdint.h>
#include <string.h>

const int kFailStatus = 67;
const int kRetryStatus = 69;

static void exitf(const char* msg, ...)
{
  int e = errno;
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  fprintf(stderr, " (errno %d)\n", e);
  doexit(kRetryStatus);
}

#define BITMASK_LEN(type, bf_len) (type)((1ull << (bf_len)) - 1)

#define BITMASK_LEN_OFF(type, bf_off, bf_len)                                  \
  (type)(BITMASK_LEN(type, (bf_len)) << (bf_off))

#define STORE_BY_BITMASK(type, addr, val, bf_off, bf_len)                      \
  if ((bf_off) == 0 && (bf_len) == 0) {                                        \
    *(type*)(addr) = (type)(val);                                              \
  } else {                                                                     \
    type new_val = *(type*)(addr);                                             \
    new_val &= ~BITMASK_LEN_OFF(type, (bf_off), (bf_len));                     \
    new_val |= ((type)(val)&BITMASK_LEN(type, (bf_len))) << (bf_off);          \
    *(type*)(addr) = new_val;                                                  \
  }

static bool write_file(const char* file, const char* what, ...)
{
  char buf[1024];
  va_list args;
  va_start(args, what);
  vsnprintf(buf, sizeof(buf), what, args);
  va_end(args);
  buf[sizeof(buf) - 1] = 0;
  int len = strlen(buf);

  int fd = open(file, O_WRONLY | O_CLOEXEC);
  if (fd == -1)
    return false;
  if (write(fd, buf, len) != len) {
    close(fd);
    return false;
  }
  close(fd);
  return true;
}

static int inject_fault(int nth)
{
  int fd;
  char buf[16];

  fd = open("/proc/thread-self/fail-nth", O_RDWR);
  if (fd == -1)
    exitf("failed to open /proc/thread-self/fail-nth");
  sprintf(buf, "%d", nth + 1);
  if (write(fd, buf, strlen(buf)) != (ssize_t)strlen(buf))
    exitf("failed to write /proc/thread-self/fail-nth");
  return fd;
}

static void test();

void loop()
{
  while (1) {
    test();
  }
}

#ifndef __NR_bpf
#define __NR_bpf 321
#endif

void test()
{
  syscall(__NR_mmap, 0x20000000, 0xfff000, 3, 0x32, -1, 0);
  *(uint32_t*)0x20903000 = 1;
  *(uint32_t*)0x20903004 = 5;
  *(uint64_t*)0x20903008 = 0x20519fa8;
  *(uint64_t*)0x20903010 = 0x20c43000;
  *(uint32_t*)0x20903018 = 0x8000;
  *(uint32_t*)0x2090301c = 0x1000;
  *(uint64_t*)0x20903020 = 0x20a97000;
  *(uint32_t*)0x20903028 = 0;
  *(uint32_t*)0x2090302c = 0;
  *(uint8_t*)0x20903030 = 0;
  *(uint8_t*)0x20903031 = 0;
  *(uint8_t*)0x20903032 = 0;
  *(uint8_t*)0x20903033 = 0;
  *(uint8_t*)0x20903034 = 0;
  *(uint8_t*)0x20903035 = 0;
  *(uint8_t*)0x20903036 = 0;
  *(uint8_t*)0x20903037 = 0;
  *(uint8_t*)0x20903038 = 0;
  *(uint8_t*)0x20903039 = 0;
  *(uint8_t*)0x2090303a = 0;
  *(uint8_t*)0x2090303b = 0;
  *(uint8_t*)0x2090303c = 0;
  *(uint8_t*)0x2090303d = 0;
  *(uint8_t*)0x2090303e = 0;
  *(uint8_t*)0x2090303f = 0;
  *(uint32_t*)0x20903040 = 0;
  *(uint8_t*)0x20519fa8 = 0x18;
  STORE_BY_BITMASK(uint8_t, 0x20519fa9, 0, 0, 4);
  STORE_BY_BITMASK(uint8_t, 0x20519fa9, 0, 4, 4);
  *(uint16_t*)0x20519faa = 0;
  *(uint32_t*)0x20519fac = 0;
  *(uint8_t*)0x20519fb0 = 0;
  *(uint8_t*)0x20519fb1 = 0;
  *(uint16_t*)0x20519fb2 = 0;
  *(uint32_t*)0x20519fb4 = 0;
  STORE_BY_BITMASK(uint8_t, 0x20519fb8, 5, 0, 3);
  STORE_BY_BITMASK(uint8_t, 0x20519fb8, 0, 3, 1);
  STORE_BY_BITMASK(uint8_t, 0x20519fb8, 0xb, 4, 4);
  STORE_BY_BITMASK(uint8_t, 0x20519fb9, 0, 0, 4);
  STORE_BY_BITMASK(uint8_t, 0x20519fb9, 0, 4, 4);
  *(uint16_t*)0x20519fba = 0;
  *(uint32_t*)0x20519fbc = 0;
  *(uint8_t*)0x20519fc8 = 0x95;
  *(uint8_t*)0x20519fc9 = 0;
  *(uint16_t*)0x20519fca = 0;
  *(uint32_t*)0x20519fcc = 0;
  memcpy((void*)0x20c43000, "syzkaller", 10);
  write_file("/sys/kernel/debug/failslab/ignore-gfp-wait", "N");
  write_file("/sys/kernel/debug/fail_futex/ignore-private", "N");
  inject_fault(22);
  syscall(__NR_bpf, 5, 0x20903000, 0x48);
}

int main()
{
  for (;;) {
    loop();
  }
}