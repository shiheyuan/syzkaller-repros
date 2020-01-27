// general protection fault in free_verifier_state (3)
// https://syzkaller.appspot.com/bug?id=7cd76b18104d09756099a580f38ace4eb4e807b7
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void sleep_ms(uint64_t ms)
{
  usleep(ms * 1000);
}

static uint64_t current_time_ms(void)
{
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts))
    exit(1);
  return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
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
    int err = errno;
    close(fd);
    errno = err;
    return false;
  }
  close(fd);
  return true;
}

static int inject_fault(int nth)
{
  int fd;
  fd = open("/proc/thread-self/fail-nth", O_RDWR);
  if (fd == -1)
    exit(1);
  char buf[16];
  sprintf(buf, "%d", nth + 1);
  if (write(fd, buf, strlen(buf)) != (ssize_t)strlen(buf))
    exit(1);
  return fd;
}

static void kill_and_wait(int pid, int* status)
{
  kill(-pid, SIGKILL);
  kill(pid, SIGKILL);
  int i;
  for (i = 0; i < 100; i++) {
    if (waitpid(-1, status, WNOHANG | __WALL) == pid)
      return;
    usleep(1000);
  }
  DIR* dir = opendir("/sys/fs/fuse/connections");
  if (dir) {
    for (;;) {
      struct dirent* ent = readdir(dir);
      if (!ent)
        break;
      if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
        continue;
      char abort[300];
      snprintf(abort, sizeof(abort), "/sys/fs/fuse/connections/%s/abort",
               ent->d_name);
      int fd = open(abort, O_WRONLY);
      if (fd == -1) {
        continue;
      }
      if (write(fd, abort, 1) < 0) {
      }
      close(fd);
    }
    closedir(dir);
  } else {
  }
  while (waitpid(-1, status, __WALL) != pid) {
  }
}

static void setup_test()
{
  prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
  setpgrp();
  write_file("/proc/self/oom_score_adj", "1000");
}

static void setup_fault()
{
  static struct {
    const char* file;
    const char* val;
    bool fatal;
  } files[] = {
      {"/sys/kernel/debug/failslab/ignore-gfp-wait", "N", true},
      {"/sys/kernel/debug/fail_futex/ignore-private", "N", false},
      {"/sys/kernel/debug/fail_page_alloc/ignore-gfp-highmem", "N", false},
      {"/sys/kernel/debug/fail_page_alloc/ignore-gfp-wait", "N", false},
      {"/sys/kernel/debug/fail_page_alloc/min-order", "0", false},
  };
  unsigned i;
  for (i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
    if (!write_file(files[i].file, files[i].val)) {
      if (files[i].fatal)
        exit(1);
    }
  }
}

static void execute_one(void);

#define WAIT_FLAGS __WALL

static void loop(void)
{
  int iter;
  for (iter = 0;; iter++) {
    int pid = fork();
    if (pid < 0)
      exit(1);
    if (pid == 0) {
      setup_test();
      execute_one();
      exit(0);
    }
    int status = 0;
    uint64_t start = current_time_ms();
    for (;;) {
      if (waitpid(-1, &status, WNOHANG | WAIT_FLAGS) == pid)
        break;
      sleep_ms(1);
      if (current_time_ms() - start < 5 * 1000)
        continue;
      kill_and_wait(pid, &status);
      break;
    }
  }
}

#ifndef __NR_bpf
#define __NR_bpf 321
#endif

void execute_one(void)
{
  *(uint32_t*)0x200017c0 = 3;
  *(uint32_t*)0x200017c4 = 0xe;
  *(uint64_t*)0x200017c8 = 0x200004c0;
  memcpy((void*)0x200004c0,
         "\xb7\x00\x00\x00\xa5\x51\x7f\x5f\xbf\xa3\x00\x00\x00\x00\x00\x00\x46"
         "\x03\x00\x00\x28\xfe\xff\xff\x7a\x0a\xf0\xff\xf8\xff\xff\xff\x71\xa4"
         "\xf0\xff\x00\x00\x00\x00\xb7\x06\x00\x00\x00\x00\x00\x01\xdd\x40\x02"
         "\x00\x00\x00\x00\x00\x24\x06\x00\x00\x01\xed\x00\x00\x71\x18\x04\x00"
         "\x00\x00\x00\x00\x2e\x64\x00\x00\x00\x00\x00\x00\x7b\x03\x00\xfe\x00"
         "\x00\x00\x00\x85\x00\x00\x00\x13\x00\x00\x00\xb7\x00\x00\x00\x00\x00"
         "\x00\x00\x95\x00\x00\x00\x00\x00\x00\x00\x02\x3b\xc0\x65\xb7\xa3\x79"
         "\xd1\x79\xfc\x9e\x88\xaf\x69\x91\x24\x35\xf1\xb6\xa6\x93\x17\x2e\x61"
         "\x91\x7a\xde\xf6\xee\x1c\x8a\x2b\x4f\x8e\xf1\xe5\x0b\xec\xa0\x90\xd3"
         "\x20\x50\xe4\x36\xfe\x27\x5d\xaf\x51\xef\xd6\x01\xb6\xbf\x01\xc8\xe8"
         "\xb1\xb5\xe4\xfe\xf3\xbe\xf7\x05\x48\x15\xae\x98\x74\x3d\x1a\xce\x4c"
         "\x46\x63\x12\x56\xdd\x19\xae\xd0\xd6\x00\xc0\x95\x19\x9f\xe3\xff\x31"
         "\x28\xe5\x99\xb0\xea\xeb\xbd\xbd\x73\x59\xa4\x8f\x5b\x0a\xfc\x64\x6c"
         "\xb7\x79\x60\x01\xb1\x47\x0d\x12\x59\x96\x5a\x72\x09\x00\x6d\x12\xcc"
         "\x8d\x46\xe5\x51\x2b\xb7\x1e\xef\x5b\x7e\xa1\x1f\xc2\x11\x42\xba\xa6"
         "\x08\xcd\x8d\x08\xb0\x97\xa5\x9f\x9f\xef\x8b\xbb\xf3\xae\x00\x0b\xe3"
         "\xdc\x3c\x12\x48\xab\xe5\x5f\x02\x00\x00\x00\x00\x00\x00\x00\x92\x3f"
         "\x3f\xee\xbe\x02\xae\x18\x23\x09\x64\x1b\x51\x85\xbd\x96\x91\xda\xeb"
         "\xc6\xb9\xdb\xfc\x31\x0d\xc8\xb1\x34\x37\x28\xb3\x7c\xd3\x16\x39\x9d"
         "\xb2\x0a\x93\xdf\x9f\x0a\x3f\xcc\x9a\x43\xc8\xee\x04\x7b\x94\xa8\x39"
         "\x56\x15\xf9\x06\x8d\xc3\x3e\x53\x62\x7a\x7e\x36\xc2\xa9\x41\xc6\xb4"
         "\x55\x3c\xed\x3b\xd7\x5c\xce\xe3\x17\x3a\x76\xec\xb5\x70\x6d\x01\x96"
         "\xe0\x29\x68\x3c\x0b\x4b\xed\x2f\x43\x8c\x1e\xa7\xda\xa6\x9f\x61\x3b"
         "\xe0\x84\x08\xc1\xbb\x4e\x1d\x26\x77\x10\xd6\xef\xf2\x09\xa9\xde\x6e"
         "\x26\x8f\xba\x34\xcf\xc8\xd7\xef\x94\x8e\x12\x98\x4f\xe7\x5a\x5d\x8d"
         "\xe5\x63\x96\x88\x70\xd4\xb9\xb5\x05\xd9\x4d\x5b\x37\x06\x34\x65\xda"
         "\xbb\xd7\x66\x0c\x2a\xb3\x74\xf2\xf4\x6c\xcd\x79\x36\xe6\xc1\x41\x88"
         "\x6e\x7c\x89\x14\xb1\x87\x9a\x06\x32\xee\x45\x2c\xeb\x9f\x7f\x61\xb1"
         "\x34\x33\x14\x3b\xf4\xda\xc5\xe7\x44\xea\x96\x6f\xab\x25\x1d\x39\x75"
         "\x55\x14\xc7\x45\x32\x33\x72\x42\x93\x93\xa6\x72\xd1\x3d\xa6\x13\x33"
         "\xec\x32\x88\xfb\x21\x38\x0c\x13\xb3\xd5\x03\x42\xcd\xed\x06\x5e\x0a"
         "\x8d\xdc\x45\x5e\x9f\x62\x77\xcf\xbf\x31\x68\x5e\xd0\xf3\x11\x39\xf4"
         "\xc0\x60\x8c\x5e\xf9\x5f\xa9\xa9\xe2\xbc\xf8\xca\x55\x09",
         558);
  *(uint64_t*)0x200017d0 = 0x200001c0;
  memcpy((void*)0x200001c0, "GPL\000", 4);
  *(uint32_t*)0x200017d8 = 0;
  *(uint32_t*)0x200017dc = 0;
  *(uint64_t*)0x200017e0 = 0;
  *(uint32_t*)0x200017e8 = 0;
  *(uint32_t*)0x200017ec = 0;
  *(uint8_t*)0x200017f0 = 0;
  *(uint8_t*)0x200017f1 = 0;
  *(uint8_t*)0x200017f2 = 0;
  *(uint8_t*)0x200017f3 = 0;
  *(uint8_t*)0x200017f4 = 0;
  *(uint8_t*)0x200017f5 = 0;
  *(uint8_t*)0x200017f6 = 0;
  *(uint8_t*)0x200017f7 = 0;
  *(uint8_t*)0x200017f8 = 0;
  *(uint8_t*)0x200017f9 = 0;
  *(uint8_t*)0x200017fa = 0;
  *(uint8_t*)0x200017fb = 0;
  *(uint8_t*)0x200017fc = 0;
  *(uint8_t*)0x200017fd = 0;
  *(uint8_t*)0x200017fe = 0;
  *(uint8_t*)0x200017ff = 0;
  *(uint32_t*)0x20001800 = 0;
  *(uint32_t*)0x20001804 = 0;
  *(uint32_t*)0x20001808 = -1;
  *(uint32_t*)0x2000180c = 8;
  *(uint64_t*)0x20001810 = 0x20000000;
  *(uint32_t*)0x20000000 = 0;
  *(uint32_t*)0x20000004 = 0;
  *(uint32_t*)0x20001818 = 0;
  *(uint32_t*)0x2000181c = 0x10;
  *(uint64_t*)0x20001820 = 0x20000000;
  *(uint32_t*)0x20000000 = 0;
  *(uint32_t*)0x20000004 = 0;
  *(uint32_t*)0x20000008 = 0;
  *(uint32_t*)0x2000000c = 0;
  *(uint32_t*)0x20001828 = 0;
  *(uint32_t*)0x2000182c = 0;
  *(uint32_t*)0x20001830 = -1;
  inject_fault(22);
  syscall(__NR_bpf, 5ul, 0x200017c0ul, 0x48ul);
}
int main(void)
{
  inject_fault(22);
  syscall(__NR_mmap, 0x20000000ul, 0x1000000ul, 3ul, 0x32ul, -1, 0);
  setup_fault();
  loop();
  return 0;
}