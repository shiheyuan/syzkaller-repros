// kernel panic: Attempted to kill init!
// https://syzkaller.appspot.com/bug?id=9e824ed38b7dc5e903c869f25eb1fa1363be2af8
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <linux/genetlink.h>
#include <linux/netlink.h>

unsigned long long procid;

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

static long syz_open_dev(long a0, long a1, long a2)
{
  if (a0 == 0xc || a0 == 0xb) {
    char buf[128];
    sprintf(buf, "/dev/%s/%d:%d", a0 == 0xc ? "char" : "block", (uint8_t)a1,
            (uint8_t)a2);
    return open(buf, O_RDWR, 0);
  } else {
    char buf[1024];
    char* hash;
    strncpy(buf, (char*)a0, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;
    while ((hash = strchr(buf, '#'))) {
      *hash = '0' + (char)(a1 % 10);
      a1 /= 10;
    }
    return open(buf, a2, 0);
  }
}

static long syz_open_procfs(long a0, long a1)
{
  char buf[128];
  memset(buf, 0, sizeof(buf));
  if (a0 == 0) {
    snprintf(buf, sizeof(buf), "/proc/self/%s", (char*)a1);
  } else if (a0 == -1) {
    snprintf(buf, sizeof(buf), "/proc/thread-self/%s", (char*)a1);
  } else {
    snprintf(buf, sizeof(buf), "/proc/self/task/%d/%s", (int)a0, (char*)a1);
  }
  int fd = open(buf, O_RDWR);
  if (fd == -1)
    fd = open(buf, O_RDONLY);
  return fd;
}

static long syz_genetlink_get_family_id(long name)
{
  char buf[512] = {0};
  struct nlmsghdr* hdr = (struct nlmsghdr*)buf;
  struct genlmsghdr* genlhdr = (struct genlmsghdr*)NLMSG_DATA(hdr);
  struct nlattr* attr = (struct nlattr*)(genlhdr + 1);
  hdr->nlmsg_len =
      sizeof(*hdr) + sizeof(*genlhdr) + sizeof(*attr) + GENL_NAMSIZ;
  hdr->nlmsg_type = GENL_ID_CTRL;
  hdr->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
  genlhdr->cmd = CTRL_CMD_GETFAMILY;
  attr->nla_type = CTRL_ATTR_FAMILY_NAME;
  attr->nla_len = sizeof(*attr) + GENL_NAMSIZ;
  strncpy((char*)(attr + 1), (char*)name, GENL_NAMSIZ);
  struct iovec iov = {hdr, hdr->nlmsg_len};
  struct sockaddr_nl addr = {0};
  addr.nl_family = AF_NETLINK;
  int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  if (fd == -1) {
    return -1;
  }
  struct msghdr msg = {&addr, sizeof(addr), &iov, 1, NULL, 0, 0};
  if (sendmsg(fd, &msg, 0) == -1) {
    close(fd);
    return -1;
  }
  ssize_t n = recv(fd, buf, sizeof(buf), 0);
  close(fd);
  if (n <= 0) {
    return -1;
  }
  if (hdr->nlmsg_type != GENL_ID_CTRL) {
    return -1;
  }
  for (; (char*)attr < buf + n;
       attr = (struct nlattr*)((char*)attr + NLMSG_ALIGN(attr->nla_len))) {
    if (attr->nla_type == CTRL_ATTR_FAMILY_ID)
      return *(uint16_t*)(attr + 1);
  }
  return -1;
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

#define SYZ_HAVE_SETUP_TEST 1
static void setup_test()
{
  prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
  setpgrp();
}

#define SYZ_HAVE_RESET_TEST 1
static void reset_test()
{
  int fd;
  for (fd = 3; fd < 30; fd++)
    close(fd);
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
      reset_test();
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

uint64_t r[5] = {0xffffffffffffffff, 0x0, 0xffffffffffffffff,
                 0xffffffffffffffff, 0xffffffffffffffff};

void execute_one(void)
{
  long res = 0;
  res = syscall(__NR_inotify_init1, 0);
  if (res != -1)
    r[0] = res;
  syscall(__NR_fcntl, r[0], 8, -1);
  res = syscall(__NR_fcntl, r[0], 0x10, 0x20000000);
  if (res != -1)
    r[1] = *(uint32_t*)0x20000004;
  syscall(__NR_ptrace, 0x4206, r[1], 0, 0x100000);
  res = syscall(__NR_socket, 0xf, 3, 2);
  if (res != -1)
    r[2] = res;
  *(uint64_t*)0x20000180 = 0;
  *(uint32_t*)0x20000188 = 0;
  *(uint64_t*)0x20000190 = 0x20000040;
  *(uint64_t*)0x20000040 = 0x20000480;
  *(uint64_t*)0x20000048 = 0;
  *(uint64_t*)0x20000198 = 1;
  *(uint64_t*)0x200001a0 = 0;
  *(uint64_t*)0x200001a8 = 0;
  *(uint32_t*)0x200001b0 = 0;
  syscall(__NR_sendmsg, -1, 0x20000180, 0);
  *(uint64_t*)0x200001c0 = 0x40000000;
  *(uint32_t*)0x200001c8 = 0;
  *(uint64_t*)0x200001d0 = 0x20000040;
  *(uint64_t*)0x20000040 = 0x20000080;
  memcpy((void*)0x20000080,
         "\x02\x03\x06\x09\x10\x00\x00\x00\x00\x00\x00\x4c\x9e\x00\x00\x00\x02"
         "\x00\x13\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x05\x00"
         "\x06\x00\x20\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00"
         "\xe5\x00\x00\x07\x00\x00\x00\x1f\x00\x00\x00\x00\x00\x00\x03\x00\x00"
         "\x00\x00\x00\x00\x02\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00"
         "\x00\x00\x00\x05\x00\x05\x00\x00\x00\x00\x00\x0a\x00\x00\x00\x00\x00"
         "\x00\x00\xff\x17\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x17\x00\x00\x00\x00\x00\x00\x00\x00",
         128);
  *(uint64_t*)0x20000048 = 0x80;
  *(uint64_t*)0x200001d8 = 1;
  *(uint64_t*)0x200001e0 = 0;
  *(uint64_t*)0x200001e8 = 0;
  *(uint32_t*)0x200001f0 = 0;
  syscall(__NR_sendmsg, -1, 0x200001c0, 0);
  syscall(__NR_sendmmsg, r[2], 0x20000180, 0x400000000000002, 0);
  memcpy((void*)0x20000040, "./file0", 8);
  syscall(__NR_creat, 0x20000040, 0);
  syscall(__NR_clone, 0x2102001ffc, 0, 0x9999999999999999, 0x20000640, -1);
  syscall(__NR_ioctl, -1, 0x801054db, 0x20000580);
  *(uint32_t*)0x20000040 = 0;
  *(uint32_t*)0x20000044 = 0;
  syscall(__NR_setsockopt, -1, 6, 0x16, 0x20000040, 1);
  memcpy((void*)0x20000600, "IPVS", 5);
  syz_genetlink_get_family_id(0x20000600);
  memcpy((void*)0x20000700, "./bus", 6);
  res = syscall(__NR_creat, 0x20000700, 0);
  if (res != -1)
    r[3] = res;
  *(uint32_t*)0x20000240 = 0xa0;
  *(uint8_t*)0x20000244 = 0x19;
  *(uint16_t*)0x20000245 = 0;
  *(uint64_t*)0x20000247 = 0;
  *(uint8_t*)0x2000024f = 0;
  *(uint32_t*)0x20000250 = 0;
  *(uint64_t*)0x20000254 = 0;
  *(uint32_t*)0x2000025c = 0;
  *(uint32_t*)0x20000260 = 0;
  *(uint32_t*)0x20000264 = 0;
  *(uint64_t*)0x20000268 = 0;
  *(uint64_t*)0x20000270 = 0;
  *(uint64_t*)0x20000278 = 0;
  *(uint64_t*)0x20000280 = 0;
  *(uint64_t*)0x20000288 = 0;
  *(uint64_t*)0x20000290 = 0;
  *(uint64_t*)0x20000298 = 0;
  *(uint64_t*)0x200002a0 = 0;
  *(uint64_t*)0x200002a8 = 0;
  *(uint64_t*)0x200002b0 = 0;
  *(uint64_t*)0x200002b8 = 0;
  *(uint64_t*)0x200002c0 = 0;
  *(uint64_t*)0x200002c8 = 0;
  *(uint64_t*)0x200002d0 = 2;
  *(uint64_t*)0x200002d8 = 0x100000000;
  syscall(__NR_write, r[3], 0x20000240, 0xa0);
  syscall(__NR_lseek, -1, 0, 3);
  res = syscall(__NR_socket, 0xa, 2, 0);
  if (res != -1)
    r[4] = res;
  *(uint16_t*)0x20000040 = 0xa;
  *(uint16_t*)0x20000042 = htobe16(0);
  *(uint32_t*)0x20000044 = 0;
  *(uint8_t*)0x20000048 = 0xfe;
  *(uint8_t*)0x20000049 = 0x80;
  *(uint8_t*)0x2000004a = 0;
  *(uint8_t*)0x2000004b = 0;
  *(uint8_t*)0x2000004c = 0;
  *(uint8_t*)0x2000004d = 0;
  *(uint8_t*)0x2000004e = 0;
  *(uint8_t*)0x2000004f = 0;
  *(uint8_t*)0x20000050 = 0;
  *(uint8_t*)0x20000051 = 0;
  *(uint8_t*)0x20000052 = 0;
  *(uint8_t*)0x20000053 = 0;
  *(uint8_t*)0x20000054 = 0;
  *(uint8_t*)0x20000055 = 0;
  *(uint8_t*)0x20000056 = 0;
  *(uint8_t*)0x20000057 = 0xaa;
  *(uint32_t*)0x20000058 = 6;
  syscall(__NR_connect, r[4], 0x20000040, 0x1c);
  *(uint32_t*)0x20000080 = 0x1000;
  syscall(__NR_getsockopt, r[4], 6, 0x3d, 0x20000100, 0x20000080);
  syscall(__NR_ioctl, -1, 0x4c06, -1);
  *(uint32_t*)0x20d06000 = 0xfffffffd;
  syscall(__NR_setsockopt, -1, 6, 0x10000000013, 0x20d06000, 4);
  syscall(__NR_ioctl, -1, 0x400c6615, 0x20000280);
  *(uint16_t*)0x20000180 = 2;
  *(uint16_t*)0x20000182 = htobe16(0);
  *(uint8_t*)0x20000184 = 0xac;
  *(uint8_t*)0x20000185 = 0x14;
  *(uint8_t*)0x20000186 = 0x14;
  *(uint8_t*)0x20000187 = 0xaa;
  *(uint8_t*)0x20000188 = 0;
  *(uint8_t*)0x20000189 = 0;
  *(uint8_t*)0x2000018a = 0;
  *(uint8_t*)0x2000018b = 0;
  *(uint8_t*)0x2000018c = 0;
  *(uint8_t*)0x2000018d = 0;
  *(uint8_t*)0x2000018e = 0;
  *(uint8_t*)0x2000018f = 0;
  syscall(__NR_connect, -1, 0x20000180, 0x10);
  syscall(__NR_shutdown, -1, 1);
  syscall(__NR_setpriority, 1, 0, 4);
  *(uint32_t*)0x2001d000 = 1;
  *(uint32_t*)0x2001d004 = 0x70;
  *(uint8_t*)0x2001d008 = 0;
  *(uint8_t*)0x2001d009 = 0;
  *(uint8_t*)0x2001d00a = 0;
  *(uint8_t*)0x2001d00b = 0;
  *(uint32_t*)0x2001d00c = 0;
  *(uint64_t*)0x2001d010 = 0x7f;
  *(uint64_t*)0x2001d018 = 0;
  *(uint64_t*)0x2001d020 = 0;
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 0, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 1, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 2, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 3, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 4, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0x81, 5, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 6, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 7, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 8, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 9, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 10, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 11, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 12, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 13, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 14, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 15, 2);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 17, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 18, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 19, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 20, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 21, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0xfffffffffffffffd, 22, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 23, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 24, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 25, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 26, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 27, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 28, 1);
  STORE_BY_BITMASK(uint64_t, 0x2001d028, 0, 29, 35);
  *(uint32_t*)0x2001d030 = 0;
  *(uint32_t*)0x2001d034 = 0;
  *(uint64_t*)0x2001d038 = 0x5ae;
  *(uint64_t*)0x2001d040 = 2;
  *(uint64_t*)0x2001d048 = 0;
  *(uint64_t*)0x2001d050 = 0;
  *(uint32_t*)0x2001d058 = 0;
  *(uint32_t*)0x2001d05c = 0;
  *(uint64_t*)0x2001d060 = 0;
  *(uint32_t*)0x2001d068 = 0;
  *(uint16_t*)0x2001d06c = 0;
  *(uint16_t*)0x2001d06e = 0;
  syscall(__NR_perf_event_open, 0x2001d000, 0, -1, -1, 1);
  syscall(__NR_ioctl, -1, 0x80404525, 0x20000200);
  syscall(__NR_recvfrom, -1, 0x20f9cf9b, 0x6478c8501c739ddb, 0, 0,
          0xffffffffffffff39);
  syscall(__NR_sendfile, -1, -1, 0, 0x6d);
  *(uint16_t*)0x200001c0 = 6;
  *(uint16_t*)0x200001c2 = 6;
  *(uint16_t*)0x200001c4 = 0x401;
  *(uint16_t*)0x200001c6 = 0;
  syscall(__NR_ioctl, -1, 0x5414, 0x200001c0);
  memcpy((void*)0x20000000, "attr", 5);
  syz_open_procfs(0, 0x20000000);
  syscall(__NR_socketpair, 1, 3, 0, 0x20000000);
  memcpy((void*)0x200000c0,
         "\x73\x79\x7a\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
         80);
  *(uint16_t*)0x20000110 = 0;
  *(uint16_t*)0x20000112 = 0;
  *(uint16_t*)0x20000114 = 0;
  *(uint16_t*)0x20000116 = 0;
  *(uint32_t*)0x20000118 = 0;
  *(uint32_t*)0x2000011c = 0;
  *(uint32_t*)0x20000120 = 0;
  *(uint32_t*)0x20000124 = 0;
  *(uint32_t*)0x20000128 = 0;
  *(uint32_t*)0x2000012c = 0;
  *(uint32_t*)0x20000130 = 0;
  *(uint32_t*)0x20000134 = 0;
  *(uint32_t*)0x20000138 = 0;
  *(uint32_t*)0x2000013c = 0;
  *(uint32_t*)0x20000140 = 0;
  *(uint32_t*)0x20000144 = 0;
  *(uint32_t*)0x20000148 = 0;
  *(uint32_t*)0x2000014c = 0;
  *(uint32_t*)0x20000150 = 0;
  *(uint32_t*)0x20000154 = 0;
  *(uint32_t*)0x20000158 = 0;
  *(uint32_t*)0x2000015c = 0;
  *(uint32_t*)0x20000160 = 0;
  *(uint32_t*)0x20000164 = 0;
  *(uint32_t*)0x20000168 = 0;
  *(uint32_t*)0x2000016c = 0;
  *(uint32_t*)0x20000170 = 0;
  *(uint32_t*)0x20000174 = 0;
  *(uint32_t*)0x20000178 = 0;
  *(uint32_t*)0x2000017c = 0;
  *(uint32_t*)0x20000180 = 0;
  *(uint32_t*)0x20000184 = 0;
  *(uint32_t*)0x20000188 = 0;
  *(uint32_t*)0x2000018c = 0;
  *(uint32_t*)0x20000190 = 0;
  *(uint32_t*)0x20000194 = 0;
  *(uint32_t*)0x20000198 = 0;
  *(uint32_t*)0x2000019c = 0;
  *(uint32_t*)0x200001a0 = 0;
  *(uint32_t*)0x200001a4 = 0;
  *(uint32_t*)0x200001a8 = 0;
  *(uint32_t*)0x200001ac = 0;
  *(uint32_t*)0x200001b0 = 0;
  *(uint32_t*)0x200001b4 = 0;
  *(uint32_t*)0x200001b8 = 0;
  *(uint32_t*)0x200001bc = 0;
  *(uint32_t*)0x200001c0 = 0;
  *(uint32_t*)0x200001c4 = 0;
  *(uint32_t*)0x200001c8 = 0;
  *(uint32_t*)0x200001cc = 0;
  *(uint32_t*)0x200001d0 = 0;
  *(uint32_t*)0x200001d4 = 0;
  *(uint32_t*)0x200001d8 = 0;
  *(uint32_t*)0x200001dc = 0;
  *(uint32_t*)0x200001e0 = 0;
  *(uint32_t*)0x200001e4 = 0;
  *(uint32_t*)0x200001e8 = 0;
  *(uint32_t*)0x200001ec = 0;
  *(uint32_t*)0x200001f0 = 0;
  *(uint32_t*)0x200001f4 = 0;
  *(uint32_t*)0x200001f8 = 0;
  *(uint32_t*)0x200001fc = 0;
  *(uint32_t*)0x20000200 = 0;
  *(uint32_t*)0x20000204 = 0;
  *(uint32_t*)0x20000208 = 0;
  *(uint32_t*)0x2000020c = 0;
  *(uint32_t*)0x20000210 = 0;
  *(uint32_t*)0x20000214 = 0;
  *(uint32_t*)0x20000218 = 0;
  *(uint32_t*)0x2000021c = 0;
  *(uint32_t*)0x20000220 = 0;
  *(uint32_t*)0x20000224 = 0;
  *(uint32_t*)0x20000228 = 0;
  *(uint32_t*)0x2000022c = 0;
  *(uint32_t*)0x20000230 = 0;
  *(uint32_t*)0x20000234 = 0;
  *(uint32_t*)0x20000238 = 0;
  *(uint32_t*)0x2000023c = 0;
  *(uint32_t*)0x20000240 = 0;
  *(uint32_t*)0x20000244 = 0;
  *(uint32_t*)0x20000248 = 0;
  *(uint32_t*)0x2000024c = 0;
  *(uint32_t*)0x20000250 = 0;
  *(uint32_t*)0x20000254 = 0;
  *(uint32_t*)0x20000258 = 0;
  *(uint32_t*)0x2000025c = 0;
  *(uint32_t*)0x20000260 = 0;
  *(uint32_t*)0x20000264 = 0;
  *(uint32_t*)0x20000268 = 0;
  *(uint32_t*)0x2000026c = 0;
  *(uint32_t*)0x20000270 = 0;
  *(uint32_t*)0x20000274 = 0;
  *(uint32_t*)0x20000278 = 0;
  *(uint32_t*)0x2000027c = 0;
  *(uint32_t*)0x20000280 = 0;
  *(uint32_t*)0x20000284 = 0;
  *(uint32_t*)0x20000288 = 0;
  *(uint32_t*)0x2000028c = 0;
  *(uint32_t*)0x20000290 = 0;
  *(uint32_t*)0x20000294 = 0;
  *(uint32_t*)0x20000298 = 0;
  *(uint32_t*)0x2000029c = 0;
  *(uint32_t*)0x200002a0 = 0;
  *(uint32_t*)0x200002a4 = 0;
  *(uint32_t*)0x200002a8 = 0;
  *(uint32_t*)0x200002ac = 0;
  *(uint32_t*)0x200002b0 = 0;
  *(uint32_t*)0x200002b4 = 0;
  *(uint32_t*)0x200002b8 = 0;
  *(uint32_t*)0x200002bc = 0;
  *(uint32_t*)0x200002c0 = 0;
  *(uint32_t*)0x200002c4 = 0;
  *(uint32_t*)0x200002c8 = 0;
  *(uint32_t*)0x200002cc = 0;
  *(uint32_t*)0x200002d0 = 0;
  *(uint32_t*)0x200002d4 = 0;
  *(uint32_t*)0x200002d8 = 0;
  *(uint32_t*)0x200002dc = 0;
  *(uint32_t*)0x200002e0 = 0;
  *(uint32_t*)0x200002e4 = 0;
  *(uint32_t*)0x200002e8 = 0;
  *(uint32_t*)0x200002ec = 0;
  *(uint32_t*)0x200002f0 = 0;
  *(uint32_t*)0x200002f4 = 0;
  *(uint32_t*)0x200002f8 = 0;
  *(uint32_t*)0x200002fc = 0;
  *(uint32_t*)0x20000300 = 0;
  *(uint32_t*)0x20000304 = 0;
  *(uint32_t*)0x20000308 = 0;
  *(uint32_t*)0x2000030c = 0;
  *(uint32_t*)0x20000310 = 0;
  *(uint32_t*)0x20000314 = 0;
  *(uint32_t*)0x20000318 = 0;
  *(uint32_t*)0x2000031c = 0;
  *(uint32_t*)0x20000320 = 0;
  *(uint32_t*)0x20000324 = 0;
  *(uint32_t*)0x20000328 = 0;
  *(uint32_t*)0x2000032c = 0;
  *(uint32_t*)0x20000330 = 0;
  *(uint32_t*)0x20000334 = 0;
  *(uint32_t*)0x20000338 = 0;
  *(uint32_t*)0x2000033c = 0;
  *(uint32_t*)0x20000340 = 0;
  *(uint32_t*)0x20000344 = 0;
  *(uint32_t*)0x20000348 = 0;
  *(uint32_t*)0x2000034c = 0;
  *(uint32_t*)0x20000350 = 0;
  *(uint32_t*)0x20000354 = 0;
  *(uint32_t*)0x20000358 = 0;
  *(uint32_t*)0x2000035c = 0;
  *(uint32_t*)0x20000360 = 0;
  *(uint32_t*)0x20000364 = 0;
  *(uint32_t*)0x20000368 = 0;
  *(uint32_t*)0x2000036c = 0;
  *(uint32_t*)0x20000370 = 0;
  *(uint32_t*)0x20000374 = 0;
  *(uint32_t*)0x20000378 = 0;
  *(uint32_t*)0x2000037c = 0;
  *(uint32_t*)0x20000380 = 0;
  *(uint32_t*)0x20000384 = 0;
  *(uint32_t*)0x20000388 = 0;
  *(uint32_t*)0x2000038c = 0;
  *(uint32_t*)0x20000390 = 0;
  *(uint32_t*)0x20000394 = 0;
  *(uint32_t*)0x20000398 = 0;
  *(uint32_t*)0x2000039c = 0;
  *(uint32_t*)0x200003a0 = 0;
  *(uint32_t*)0x200003a4 = 0;
  *(uint32_t*)0x200003a8 = 0;
  *(uint32_t*)0x200003ac = 0;
  *(uint32_t*)0x200003b0 = 0;
  *(uint32_t*)0x200003b4 = 0;
  *(uint32_t*)0x200003b8 = 0;
  *(uint32_t*)0x200003bc = 0;
  *(uint32_t*)0x200003c0 = 0;
  *(uint32_t*)0x200003c4 = 0;
  *(uint32_t*)0x200003c8 = 0;
  *(uint32_t*)0x200003cc = 0;
  *(uint32_t*)0x200003d0 = 0;
  *(uint32_t*)0x200003d4 = 0;
  *(uint32_t*)0x200003d8 = 0;
  *(uint32_t*)0x200003dc = 0;
  *(uint32_t*)0x200003e0 = 0;
  *(uint32_t*)0x200003e4 = 0;
  *(uint32_t*)0x200003e8 = 0;
  *(uint32_t*)0x200003ec = 0;
  *(uint32_t*)0x200003f0 = 0;
  *(uint32_t*)0x200003f4 = 0;
  *(uint32_t*)0x200003f8 = 0;
  *(uint32_t*)0x200003fc = 0;
  *(uint32_t*)0x20000400 = 0;
  *(uint32_t*)0x20000404 = 0;
  *(uint32_t*)0x20000408 = 0;
  *(uint32_t*)0x2000040c = 0;
  *(uint32_t*)0x20000410 = 0;
  *(uint32_t*)0x20000414 = 0;
  *(uint32_t*)0x20000418 = 0;
  *(uint32_t*)0x2000041c = 0;
  *(uint32_t*)0x20000420 = 0;
  *(uint32_t*)0x20000424 = 0;
  *(uint32_t*)0x20000428 = 0;
  *(uint32_t*)0x2000042c = 0;
  *(uint32_t*)0x20000430 = 0;
  *(uint32_t*)0x20000434 = 0;
  *(uint32_t*)0x20000438 = 0;
  *(uint32_t*)0x2000043c = 0;
  *(uint32_t*)0x20000440 = 0;
  *(uint32_t*)0x20000444 = 0;
  *(uint32_t*)0x20000448 = 0;
  *(uint32_t*)0x2000044c = 0;
  *(uint32_t*)0x20000450 = 0;
  *(uint32_t*)0x20000454 = 0;
  *(uint32_t*)0x20000458 = 0;
  *(uint32_t*)0x2000045c = 0;
  *(uint32_t*)0x20000460 = 0;
  *(uint32_t*)0x20000464 = 0;
  *(uint32_t*)0x20000468 = 0;
  *(uint32_t*)0x2000046c = 0;
  *(uint32_t*)0x20000470 = 0;
  *(uint32_t*)0x20000474 = 0;
  *(uint32_t*)0x20000478 = 0;
  *(uint32_t*)0x2000047c = 0;
  *(uint32_t*)0x20000480 = 0;
  *(uint32_t*)0x20000484 = 0;
  *(uint32_t*)0x20000488 = 0;
  *(uint32_t*)0x2000048c = 0;
  *(uint32_t*)0x20000490 = 0;
  *(uint32_t*)0x20000494 = 0;
  *(uint32_t*)0x20000498 = 0;
  *(uint32_t*)0x2000049c = 0;
  *(uint32_t*)0x200004a0 = 0;
  *(uint32_t*)0x200004a4 = 0;
  *(uint32_t*)0x200004a8 = 0;
  *(uint32_t*)0x200004ac = 0;
  *(uint32_t*)0x200004b0 = 0;
  *(uint32_t*)0x200004b4 = 0;
  *(uint32_t*)0x200004b8 = 0;
  *(uint32_t*)0x200004bc = 0;
  *(uint32_t*)0x200004c0 = 0;
  *(uint32_t*)0x200004c4 = 0;
  *(uint32_t*)0x200004c8 = 0;
  *(uint32_t*)0x200004cc = 0;
  *(uint32_t*)0x200004d0 = 0;
  *(uint32_t*)0x200004d4 = 0;
  *(uint32_t*)0x200004d8 = 0;
  *(uint32_t*)0x200004dc = 0;
  *(uint32_t*)0x200004e0 = 0;
  *(uint32_t*)0x200004e4 = 0;
  *(uint32_t*)0x200004e8 = 0;
  *(uint32_t*)0x200004ec = 0;
  *(uint32_t*)0x200004f0 = 0;
  *(uint32_t*)0x200004f4 = 0;
  *(uint32_t*)0x200004f8 = 0;
  *(uint32_t*)0x200004fc = 0;
  *(uint32_t*)0x20000500 = 0;
  *(uint32_t*)0x20000504 = 0;
  *(uint32_t*)0x20000508 = 0;
  *(uint32_t*)0x2000050c = 0;
  *(uint32_t*)0x20000510 = 0;
  *(uint32_t*)0x20000514 = 0;
  *(uint32_t*)0x20000518 = 0;
  syscall(__NR_write, -1, 0x200000c0, 0x45c);
  syscall(__NR_ioctl, -1, 0x40045564, 0x15);
  *(uint16_t*)0x20000780 = 1;
  *(uint8_t*)0x20000782 = 0;
  *(uint32_t*)0x20000784 = 0x4e20;
  syscall(__NR_connect, -1, 0x20000780, 0x6e);
  *(uint32_t*)0x20940000 = 2;
  *(uint32_t*)0x20940004 = 0x70;
  *(uint8_t*)0x20940008 = 0x6a;
  *(uint8_t*)0x20940009 = 0;
  *(uint8_t*)0x2094000a = 0;
  *(uint8_t*)0x2094000b = 0;
  *(uint32_t*)0x2094000c = 0;
  *(uint64_t*)0x20940010 = 0;
  *(uint64_t*)0x20940018 = 0;
  *(uint64_t*)0x20940020 = 0;
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 0, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 1, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 2, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 3, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 4, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 5, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 6, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 7, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 8, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 9, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 10, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 11, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 12, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 13, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 14, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 15, 2);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 17, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 18, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 19, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 20, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 21, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 22, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 23, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 24, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 25, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 26, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 27, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 28, 1);
  STORE_BY_BITMASK(uint64_t, 0x20940028, 0, 29, 35);
  *(uint32_t*)0x20940030 = 0;
  *(uint32_t*)0x20940034 = 0;
  *(uint64_t*)0x20940038 = 0x20000000;
  *(uint64_t*)0x20940040 = 0;
  *(uint64_t*)0x20940048 = 0;
  *(uint64_t*)0x20940050 = 0;
  *(uint32_t*)0x20940058 = 0;
  *(uint32_t*)0x2094005c = 0;
  *(uint64_t*)0x20940060 = 0;
  *(uint32_t*)0x20940068 = 0;
  *(uint16_t*)0x2094006c = 0;
  *(uint16_t*)0x2094006e = 0;
  syscall(__NR_perf_event_open, 0x20940000, 0, 0, -1, 0);
  syscall(__NR_ioctl, -1, 0x5501);
  syscall(__NR_ioctl, -1, 0x4004556b, 0x64);
  *(uint16_t*)0x20000400 = 0;
  *(uint16_t*)0x20000402 = 0;
  *(uint16_t*)0x20000404 = 0;
  *(uint16_t*)0x20000406 = 0;
  memcpy((void*)0x20000408,
         "\x73\x79\x7a\x31\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
         80);
  *(uint32_t*)0x20000458 = 0x57;
  syscall(__NR_ioctl, -1, 0x405c5503, 0x20000400);
  syscall(__NR_socketpair, 1, 3, 0, 0x200000c0);
  memcpy((void*)0x20000000, "hugetlb.2MB.usage_in_bytes", 27);
  syscall(__NR_openat, -1, 0x20000000, 0, 0);
  syscall(__NR_clone, 0x20002100, 0, 0x9999999999999999, 0x20000100, -1);
  *(uint32_t*)0x200001c0 = 0;
  *(uint32_t*)0x200001c4 = 0;
  syscall(__NR_ioctl, -1, 0x4b4c, 0x200001c0);
  syscall(__NR_ioctl, -1, 0x8904, 0x20000580);
  syscall(__NR_ioprio_get, 3, 0);
  syscall(__NR_ioctl, -1, 0x5415, 0x20000340);
  *(uint32_t*)0x200002c0 = 2;
  *(uint32_t*)0x200002c4 = 0x70;
  *(uint8_t*)0x200002c8 = -1;
  *(uint8_t*)0x200002c9 = 3;
  *(uint8_t*)0x200002ca = 0;
  *(uint8_t*)0x200002cb = 0;
  *(uint32_t*)0x200002cc = 0;
  *(uint64_t*)0x200002d0 = 0;
  *(uint64_t*)0x200002d8 = 0;
  *(uint64_t*)0x200002e0 = 6;
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 2, 0, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 1, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 2, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 1, 3, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 4, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0xfffffffffffffffa, 5, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 6, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 8, 7, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 8, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0x400, 9, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 9, 10, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 11, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 5, 12, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 13, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 14, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 15, 2);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 17, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 18, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 19, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 7, 20, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 21, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0xffffffffffffffc0, 22, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 2, 23, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0x7f, 24, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0x1000, 25, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0x104, 26, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0x7a, 27, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 28, 1);
  STORE_BY_BITMASK(uint64_t, 0x200002e8, 0, 29, 35);
  *(uint32_t*)0x200002f0 = 0xfffffffb;
  *(uint32_t*)0x200002f4 = 4;
  *(uint64_t*)0x200002f8 = 0x20000280;
  *(uint64_t*)0x20000300 = 0;
  *(uint64_t*)0x20000308 = 0;
  *(uint64_t*)0x20000310 = 0xe057;
  *(uint32_t*)0x20000318 = 1;
  *(uint32_t*)0x2000031c = 6;
  *(uint64_t*)0x20000320 = 0;
  *(uint32_t*)0x20000328 = 0;
  *(uint16_t*)0x2000032c = 0x80;
  *(uint16_t*)0x2000032e = 0;
  syscall(__NR_perf_event_open, 0x200002c0, 0, 3, -1, 0);
  *(uint32_t*)0x20000040 = 0;
  *(uint32_t*)0x20000044 = 0x70;
  *(uint8_t*)0x20000048 = 0;
  *(uint8_t*)0x20000049 = 0;
  *(uint8_t*)0x2000004a = 0;
  *(uint8_t*)0x2000004b = 0;
  *(uint32_t*)0x2000004c = 0;
  *(uint64_t*)0x20000050 = 0x50d;
  *(uint64_t*)0x20000058 = 0;
  *(uint64_t*)0x20000060 = 0;
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 0, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 1, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 2, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 3, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 4, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 5, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 6, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 7, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 8, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 9, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 10, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 11, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 12, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 13, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 14, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 15, 2);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 17, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 18, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 19, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 20, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 21, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 22, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 23, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 24, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 25, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 26, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 27, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 28, 1);
  STORE_BY_BITMASK(uint64_t, 0x20000068, 0, 29, 35);
  *(uint32_t*)0x20000070 = 0;
  *(uint32_t*)0x20000074 = 0;
  *(uint64_t*)0x20000078 = 0x20000000;
  *(uint64_t*)0x20000080 = 0;
  *(uint64_t*)0x20000088 = 0;
  *(uint64_t*)0x20000090 = 0;
  *(uint32_t*)0x20000098 = 0;
  *(uint32_t*)0x2000009c = 0;
  *(uint64_t*)0x200000a0 = 0;
  *(uint32_t*)0x200000a8 = 0;
  *(uint16_t*)0x200000ac = 0;
  *(uint16_t*)0x200000ae = 0;
  syscall(__NR_perf_event_open, 0x20000040, 0, 0, -1, 0);
  memcpy((void*)0x20000400, "./file0", 8);
  syscall(__NR_inotify_add_watch, -1, 0x20000400, 0);
  memcpy((void*)0x20000640, "/dev/loop#", 11);
  syz_open_dev(0x20000640, 0, 0);
  syscall(__NR_clone, 0, 0x20000240, 0x20000380, 0x200003c0, 0x20002200);
  syscall(__NR_getsockopt, -1, 1, 0x19, 0x20000200, 0x10);
  *(uint32_t*)0x200004c0 = 0xa0;
  *(uint8_t*)0x200004c4 = 0x19;
  *(uint16_t*)0x200004c5 = 0;
  *(uint64_t*)0x200004c7 = 0;
  *(uint8_t*)0x200004cf = 0;
  *(uint32_t*)0x200004d0 = 3;
  *(uint64_t*)0x200004d4 = 0;
  *(uint32_t*)0x200004dc = 0;
  *(uint32_t*)0x200004e0 = 0;
  *(uint32_t*)0x200004e4 = 0;
  *(uint64_t*)0x200004e8 = 0;
  *(uint64_t*)0x200004f0 = 2;
  *(uint64_t*)0x200004f8 = 2;
  *(uint64_t*)0x20000500 = 8;
  *(uint64_t*)0x20000508 = 0x1ff;
  *(uint64_t*)0x20000510 = 0;
  *(uint64_t*)0x20000518 = 0xfff;
  *(uint64_t*)0x20000520 = 0;
  *(uint64_t*)0x20000528 = 0x101;
  *(uint64_t*)0x20000530 = 6;
  *(uint64_t*)0x20000538 = 4;
  *(uint64_t*)0x20000540 = 8;
  *(uint64_t*)0x20000548 = 0;
  *(uint64_t*)0x20000550 = 0;
  *(uint64_t*)0x20000558 = 0;
  syscall(__NR_write, -1, 0x200004c0, 0xa0);
  memcpy((void*)0x200000c0, "tls", 4);
  syscall(__NR_setsockopt, -1, 6, 0x1f, 0x200000c0, 4);
  *(uint64_t*)0x20000180 = 0;
  syscall(__NR_sendfile, -1, -1, 0x20000180, 0x2000000000005);
  syscall(__NR_mlockall, 4);
  syscall(__NR_getpgid, 0);
  syscall(__NR_ioctl, -1, 0x8904, 0x200001c0);
}
int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  for (procid = 0; procid < 6; procid++) {
    if (fork() == 0) {
      loop();
    }
  }
  sleep(1000000);
  return 0;
}