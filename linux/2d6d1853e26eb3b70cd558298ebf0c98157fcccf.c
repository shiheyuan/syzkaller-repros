// general protection fault in skb_unlink
// https://syzkaller.appspot.com/bug?id=2d6d1853e26eb3b70cd558298ebf0c98157fcccf
// status:open
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

__attribute__((noreturn)) static void doexit(int status)
{
  volatile unsigned i;
  syscall(__NR_exit_group, status);
  for (i = 0;; i++) {
  }
}
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

const int kFailStatus = 67;
const int kRetryStatus = 69;

static void fail(const char* msg, ...)
{
  int e = errno;
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  fprintf(stderr, " (errno %d)\n", e);
  doexit((e == ENOMEM || e == EAGAIN) ? kRetryStatus : kFailStatus);
}

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

static uint64_t current_time_ms()
{
  struct timespec ts;

  if (clock_gettime(CLOCK_MONOTONIC, &ts))
    fail("clock_gettime failed");
  return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static void use_temporary_dir()
{
  char tmpdir_template[] = "./syzkaller.XXXXXX";
  char* tmpdir = mkdtemp(tmpdir_template);
  if (!tmpdir)
    fail("failed to mkdtemp");
  if (chmod(tmpdir, 0777))
    fail("failed to chmod");
  if (chdir(tmpdir))
    fail("failed to chdir");
}

static void remove_dir(const char* dir)
{
  DIR* dp;
  struct dirent* ep;
  int iter = 0;
retry:
  while (umount2(dir, MNT_DETACH) == 0) {
  }
  dp = opendir(dir);
  if (dp == NULL) {
    if (errno == EMFILE) {
      exitf("opendir(%s) failed due to NOFILE, exiting", dir);
    }
    exitf("opendir(%s) failed", dir);
  }
  while ((ep = readdir(dp))) {
    if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
      continue;
    char filename[FILENAME_MAX];
    snprintf(filename, sizeof(filename), "%s/%s", dir, ep->d_name);
    struct stat st;
    if (lstat(filename, &st))
      exitf("lstat(%s) failed", filename);
    if (S_ISDIR(st.st_mode)) {
      remove_dir(filename);
      continue;
    }
    int i;
    for (i = 0;; i++) {
      if (unlink(filename) == 0)
        break;
      if (errno == EROFS) {
        break;
      }
      if (errno != EBUSY || i > 100)
        exitf("unlink(%s) failed", filename);
      if (umount2(filename, MNT_DETACH))
        exitf("umount(%s) failed", filename);
    }
  }
  closedir(dp);
  int i;
  for (i = 0;; i++) {
    if (rmdir(dir) == 0)
      break;
    if (i < 100) {
      if (errno == EROFS) {
        break;
      }
      if (errno == EBUSY) {
        if (umount2(dir, MNT_DETACH))
          exitf("umount(%s) failed", dir);
        continue;
      }
      if (errno == ENOTEMPTY) {
        if (iter < 100) {
          iter++;
          goto retry;
        }
      }
    }
    exitf("rmdir(%s) failed", dir);
  }
}

static void execute_one();
extern unsigned long long procid;

static void loop()
{
  int iter;
  for (iter = 0;; iter++) {
    char cwdbuf[32];
    sprintf(cwdbuf, "./%d", iter);
    if (mkdir(cwdbuf, 0777))
      fail("failed to mkdir");
    int pid = fork();
    if (pid < 0)
      fail("clone failed");
    if (pid == 0) {
      prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
      setpgrp();
      if (chdir(cwdbuf))
        fail("failed to chdir");
      execute_one();
      doexit(0);
    }

    int status = 0;
    uint64_t start = current_time_ms();
    for (;;) {
      int res = waitpid(-1, &status, __WALL | WNOHANG);
      if (res == pid) {
        break;
      }
      usleep(1000);
      if (current_time_ms() - start < 3 * 1000)
        continue;
      kill(-pid, SIGKILL);
      kill(pid, SIGKILL);
      while (waitpid(-1, &status, __WALL) != pid) {
      }
      break;
    }
    remove_dir(cwdbuf);
  }
}

#ifndef __NR_bpf
#define __NR_bpf 321
#endif

uint64_t r[5] = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                 0xffffffffffffffff, 0x0};
unsigned long long procid;
void execute_one()
{
  long res = 0;
  res = syscall(__NR_socket, 2, 1, 0);
  if (res != -1)
    r[0] = res;
  *(uint16_t*)0x20e5b000 = 2;
  *(uint16_t*)0x20e5b002 = htobe16(0x4e20);
  *(uint32_t*)0x20e5b004 = htobe32(0xe0000001);
  *(uint8_t*)0x20e5b008 = 0;
  *(uint8_t*)0x20e5b009 = 0;
  *(uint8_t*)0x20e5b00a = 0;
  *(uint8_t*)0x20e5b00b = 0;
  *(uint8_t*)0x20e5b00c = 0;
  *(uint8_t*)0x20e5b00d = 0;
  *(uint8_t*)0x20e5b00e = 0;
  *(uint8_t*)0x20e5b00f = 0;
  syscall(__NR_bind, r[0], 0x20e5b000, 0x10);
  *(uint16_t*)0x20ccb000 = 2;
  *(uint16_t*)0x20ccb002 = htobe16(0x4e20);
  *(uint32_t*)0x20ccb004 = htobe32(0);
  *(uint8_t*)0x20ccb008 = 0;
  *(uint8_t*)0x20ccb009 = 0;
  *(uint8_t*)0x20ccb00a = 0;
  *(uint8_t*)0x20ccb00b = 0;
  *(uint8_t*)0x20ccb00c = 0;
  *(uint8_t*)0x20ccb00d = 0;
  *(uint8_t*)0x20ccb00e = 0;
  *(uint8_t*)0x20ccb00f = 0;
  syscall(__NR_connect, r[0], 0x20ccb000, 0x10);
  *(uint32_t*)0x20000100 = 1;
  *(uint32_t*)0x20000104 = 3;
  *(uint64_t*)0x20000108 = 0x209ff000;
  *(uint8_t*)0x209ff000 = 0x18;
  STORE_BY_BITMASK(uint8_t, 0x209ff001, 0, 0, 4);
  STORE_BY_BITMASK(uint8_t, 0x209ff001, 0, 4, 4);
  *(uint16_t*)0x209ff002 = 0;
  *(uint32_t*)0x209ff004 = 3;
  *(uint8_t*)0x209ff008 = 0;
  *(uint8_t*)0x209ff009 = 0;
  *(uint16_t*)0x209ff00a = 0;
  *(uint32_t*)0x209ff00c = 0;
  *(uint8_t*)0x209ff010 = 0x95;
  *(uint8_t*)0x209ff011 = 0;
  *(uint16_t*)0x209ff012 = 0;
  *(uint32_t*)0x209ff014 = 0;
  *(uint64_t*)0x20000110 = 0x202bf000;
  memcpy((void*)0x202bf000, "syzkaller", 10);
  *(uint32_t*)0x20000118 = 4;
  *(uint32_t*)0x2000011c = 0xb7;
  *(uint64_t*)0x20000120 = 0x20000040;
  *(uint32_t*)0x20000128 = 0;
  *(uint32_t*)0x2000012c = 0;
  *(uint8_t*)0x20000130 = 0;
  *(uint8_t*)0x20000131 = 0;
  *(uint8_t*)0x20000132 = 0;
  *(uint8_t*)0x20000133 = 0;
  *(uint8_t*)0x20000134 = 0;
  *(uint8_t*)0x20000135 = 0;
  *(uint8_t*)0x20000136 = 0;
  *(uint8_t*)0x20000137 = 0;
  *(uint8_t*)0x20000138 = 0;
  *(uint8_t*)0x20000139 = 0;
  *(uint8_t*)0x2000013a = 0;
  *(uint8_t*)0x2000013b = 0;
  *(uint8_t*)0x2000013c = 0;
  *(uint8_t*)0x2000013d = 0;
  *(uint8_t*)0x2000013e = 0;
  *(uint8_t*)0x2000013f = 0;
  *(uint32_t*)0x20000140 = 0;
  *(uint32_t*)0x20000144 = 0;
  res = syscall(__NR_bpf, 5, 0x20000100, 0x48);
  if (res != -1)
    r[1] = res;
  res = syscall(__NR_socket, 0x29, 0x1000000000002, 0);
  if (res != -1)
    r[2] = res;
  *(uint32_t*)0x2031aff8 = r[0];
  *(uint32_t*)0x2031affc = r[1];
  syscall(__NR_ioctl, r[2], 0x89e0, 0x2031aff8);
  memcpy(
      (void*)0x20000980,
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x55\x83\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xb7\xac\x7d\xdc\x1c\xda\x95\xe6"
      "\x7a\x69\xe6\x7e\x4c\x14\x0e\xb4\xc9\x76\xe5\x49\xaf\x9b\xee\x5b\x45\x82"
      "\xaf\xbb\x98\xfc\xf4\x44\xc7\x99\xa6\x09\xd2\x00\x3d\x4a\x86\x99\x9d\x92"
      "\x61\x7e\x40\x4d\xfd\xe9\x31\x0a\xaa\x46\x22\x0f\x2b\xb1\x97\x4e\xd7\xf3"
      "\xcd\x56\xa4\x91\x99\x71\xf3\x29\x3d\x91\x45\x45\x3c\xf0\x54\xe3\xb1\x5c"
      "\xe7\x1b\x87\x23\x53\xb9\x12\x30\xfd\x57\xc9\x83\x9c\xc9\x24\x1b\xff\x25"
      "\x56\x97\x7c\x66\x81\xe0\xcf\x46\xbb\x84\xf5\x02\x8b\x63\x43\x60\x88\xae"
      "\x92\x6c\xb6\x3a\xa9\x1a\x46\xa5\x37\x30\x5d\xf1\xc9\xd6\x35\x8a\x0a\xd0"
      "\x03\xbc\xbe\xc0\x88\xe2\x8d\x69\xc7\x8d\x60\x1f\xe6\x46\xe9\x57\x0b\x9e"
      "\xda\x08\x03\x25\xd4\x37\xb9\x0c\xdd\xae\xf0\x21\xd0\x07\xad\x6f\x18\x2d"
      "\xdc\xec\x52\x00\x73\x72\x38\xc3\x7d\xaa\xef\xb3\x4d\xeb\x7c\xda\x2a\x46"
      "\x0a\x36\xbf\x0c\x6b\x19\x13\x2a\xab\x71\xad\x3d\x20\xe3\xb1\x7a\x65\xbe"
      "\x1c\x71\xcd\xd5\x47\x77\x31\x4b\x5c\x54\xe3\x72\x55\xe0\x67\x09\xe3\xb9"
      "\x73\x38\x91\x68\xd9\xb6\x5c\xb6\x33\xc2\xfa\xb6\x08\x1a\x79\x28\xd8\x2b"
      "\x96\xa4\xc7\xea\xad\x10\x4c\x35\x27\x23\xa9\x23\x65\x09\xd5\x77\x4b\x1b"
      "\xd9\x8f\xc6\x4e\x75\x43\x16\xc8\x88\x6a\x89\xac\xb8\x5a\xea\x1b\xeb\x92"
      "\xd1\xb6\x83\xb0\xf8\x04\xf3\xac\x96\x9d\x6a\x81\xb3\xb4\x3d\x93\xf0\xf7"
      "\xb5\x50\x67\xf7\xad\x3b\x95\xfb\xb8\xe2\xd3\xeb\xd9\x85\x9b\xe7\xb1\xc8"
      "\xf3\x7b\x0b\xe5\xcf\x10\xcf\x28\xcb\x69\x3d\x2c\x58\xea\x2a\x32\xaa\x25"
      "\x2c\x5b\x97\x0f\x01\x1f\x2b\x77\x01\x19\x39\x73\x71\x4d\x72\x82\x3c\xb9"
      "\x3d\x28\x27\xc7\x6f\x73\xe3\xc6\x12\xea\xd4\xaf\x9f\x11",
      1166);
  syscall(__NR_write, r[0], 0x20000980, 0x48e);
  *(uint32_t*)0x20000280 = r[2];
  res = syscall(__NR_ioctl, r[2], 0x89e2, 0x20000280);
  if (res != -1)
    r[3] = *(uint32_t*)0x20000280;
  *(uint32_t*)0x20000000 = 7;
  syscall(__NR_setsockopt, r[3], 0x119, 1, 0x20000000, 4);
  res = syscall(__NR_clock_gettime, 0, 0x20000380);
  if (res != -1)
    r[4] = *(uint64_t*)0x20000388;
  *(uint64_t*)0x20000200 = 0;
  *(uint64_t*)0x20000208 = r[4] + 10000000;
  syscall(__NR_recvmmsg, r[3], 0x200002c0, 0x40000f7, 0x60, 0x20000200);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  char* cwd = get_current_dir_name();
  for (procid = 0; procid < 8; procid++) {
    if (fork() == 0) {
      for (;;) {
        if (chdir(cwd))
          fail("failed to chdir");
        use_temporary_dir();
        loop();
      }
    }
  }
  sleep(1000000);
  return 0;
}