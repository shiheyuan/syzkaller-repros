// KASAN: use-after-free Read in __kernfs_remove
// https://syzkaller.appspot.com/bug?id=e5e5257237027122be123d997f6239ed3f0b3e81
// status:open
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/loop.h>
#include <linux/net.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
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

extern unsigned long long procid;

struct fs_image_segment {
  void* data;
  uintptr_t size;
  uintptr_t offset;
};

#define IMAGE_MAX_SEGMENTS 4096
#define IMAGE_MAX_SIZE (129 << 20)

#define SYZ_memfd_create 319

static uintptr_t syz_mount_image(uintptr_t fs, uintptr_t dir, uintptr_t size,
                                 uintptr_t nsegs, uintptr_t segments,
                                 uintptr_t flags, uintptr_t opts)
{
  char loopname[64];
  int loopfd, err = 0, res = -1;
  uintptr_t i;
  struct fs_image_segment* segs = (struct fs_image_segment*)segments;

  if (nsegs > IMAGE_MAX_SEGMENTS)
    nsegs = IMAGE_MAX_SEGMENTS;
  for (i = 0; i < nsegs; i++) {
    if (segs[i].size > IMAGE_MAX_SIZE)
      segs[i].size = IMAGE_MAX_SIZE;
    segs[i].offset %= IMAGE_MAX_SIZE;
    if (segs[i].offset > IMAGE_MAX_SIZE - segs[i].size)
      segs[i].offset = IMAGE_MAX_SIZE - segs[i].size;
    if (size < segs[i].offset + segs[i].offset)
      size = segs[i].offset + segs[i].offset;
  }
  if (size > IMAGE_MAX_SIZE)
    size = IMAGE_MAX_SIZE;
  int memfd = syscall(SYZ_memfd_create, "syz_mount_image", 0);
  if (memfd == -1) {
    err = errno;
    goto error;
  }
  if (ftruncate(memfd, size)) {
    err = errno;
    goto error_close_memfd;
  }
  for (i = 0; i < nsegs; i++) {
    if (pwrite(memfd, segs[i].data, segs[i].size, segs[i].offset) < 0) {
    }
  }
  snprintf(loopname, sizeof(loopname), "/dev/loop%llu", procid);
  loopfd = open(loopname, O_RDWR);
  if (loopfd == -1) {
    err = errno;
    goto error_close_memfd;
  }
  if (ioctl(loopfd, LOOP_SET_FD, memfd)) {
    if (errno != EBUSY) {
      err = errno;
      goto error_close_loop;
    }
    ioctl(loopfd, LOOP_CLR_FD, 0);
    usleep(1000);
    if (ioctl(loopfd, LOOP_SET_FD, memfd)) {
      err = errno;
      goto error_close_loop;
    }
  }
  mkdir((char*)dir, 0777);
  if (strcmp((char*)fs, "iso9660") == 0)
    flags |= MS_RDONLY;
  if (mount(loopname, (char*)dir, (char*)fs, flags, (char*)opts)) {
    err = errno;
    goto error_clear_loop;
  }
  res = 0;
error_clear_loop:
  ioctl(loopfd, LOOP_CLR_FD, 0);
error_close_loop:
  close(loopfd);
error_close_memfd:
  close(memfd);
error:
  errno = err;
  return res;
}

#define XT_TABLE_SIZE 1536
#define XT_MAX_ENTRIES 10

struct xt_counters {
  uint64_t pcnt, bcnt;
};

struct ipt_getinfo {
  char name[32];
  unsigned int valid_hooks;
  unsigned int hook_entry[5];
  unsigned int underflow[5];
  unsigned int num_entries;
  unsigned int size;
};

struct ipt_get_entries {
  char name[32];
  unsigned int size;
  void* entrytable[XT_TABLE_SIZE / sizeof(void*)];
};

struct ipt_replace {
  char name[32];
  unsigned int valid_hooks;
  unsigned int num_entries;
  unsigned int size;
  unsigned int hook_entry[5];
  unsigned int underflow[5];
  unsigned int num_counters;
  struct xt_counters* counters;
  char entrytable[XT_TABLE_SIZE];
};

struct ipt_table_desc {
  const char* name;
  struct ipt_getinfo info;
  struct ipt_replace replace;
};

static struct ipt_table_desc ipv4_tables[] = {
    {.name = "filter"}, {.name = "nat"},      {.name = "mangle"},
    {.name = "raw"},    {.name = "security"},
};

static struct ipt_table_desc ipv6_tables[] = {
    {.name = "filter"}, {.name = "nat"},      {.name = "mangle"},
    {.name = "raw"},    {.name = "security"},
};

#define IPT_BASE_CTL 64
#define IPT_SO_SET_REPLACE (IPT_BASE_CTL)
#define IPT_SO_GET_INFO (IPT_BASE_CTL)
#define IPT_SO_GET_ENTRIES (IPT_BASE_CTL + 1)

struct arpt_getinfo {
  char name[32];
  unsigned int valid_hooks;
  unsigned int hook_entry[3];
  unsigned int underflow[3];
  unsigned int num_entries;
  unsigned int size;
};

struct arpt_get_entries {
  char name[32];
  unsigned int size;
  void* entrytable[XT_TABLE_SIZE / sizeof(void*)];
};

struct arpt_replace {
  char name[32];
  unsigned int valid_hooks;
  unsigned int num_entries;
  unsigned int size;
  unsigned int hook_entry[3];
  unsigned int underflow[3];
  unsigned int num_counters;
  struct xt_counters* counters;
  char entrytable[XT_TABLE_SIZE];
};

struct arpt_table_desc {
  const char* name;
  struct arpt_getinfo info;
  struct arpt_replace replace;
};

static struct arpt_table_desc arpt_tables[] = {
    {.name = "filter"},
};

#define ARPT_BASE_CTL 96
#define ARPT_SO_SET_REPLACE (ARPT_BASE_CTL)
#define ARPT_SO_GET_INFO (ARPT_BASE_CTL)
#define ARPT_SO_GET_ENTRIES (ARPT_BASE_CTL + 1)

static void checkpoint_iptables(struct ipt_table_desc* tables, int num_tables,
                                int family, int level)
{
  struct ipt_get_entries entries;
  socklen_t optlen;
  int fd, i;

  fd = socket(family, SOCK_STREAM, IPPROTO_TCP);
  if (fd == -1) {
    switch (errno) {
    case EAFNOSUPPORT:
    case ENOPROTOOPT:
      return;
    }
    fail("socket(%d, SOCK_STREAM, IPPROTO_TCP)", family);
  }
  for (i = 0; i < num_tables; i++) {
    struct ipt_table_desc* table = &tables[i];
    strcpy(table->info.name, table->name);
    strcpy(table->replace.name, table->name);
    optlen = sizeof(table->info);
    if (getsockopt(fd, level, IPT_SO_GET_INFO, &table->info, &optlen)) {
      switch (errno) {
      case EPERM:
      case ENOENT:
      case ENOPROTOOPT:
        continue;
      }
      fail("getsockopt(IPT_SO_GET_INFO)");
    }
    if (table->info.size > sizeof(table->replace.entrytable))
      fail("table size is too large: %u", table->info.size);
    if (table->info.num_entries > XT_MAX_ENTRIES)
      fail("too many counters: %u", table->info.num_entries);
    memset(&entries, 0, sizeof(entries));
    strcpy(entries.name, table->name);
    entries.size = table->info.size;
    optlen = sizeof(entries) - sizeof(entries.entrytable) + table->info.size;
    if (getsockopt(fd, level, IPT_SO_GET_ENTRIES, &entries, &optlen))
      fail("getsockopt(IPT_SO_GET_ENTRIES)");
    table->replace.valid_hooks = table->info.valid_hooks;
    table->replace.num_entries = table->info.num_entries;
    table->replace.size = table->info.size;
    memcpy(table->replace.hook_entry, table->info.hook_entry,
           sizeof(table->replace.hook_entry));
    memcpy(table->replace.underflow, table->info.underflow,
           sizeof(table->replace.underflow));
    memcpy(table->replace.entrytable, entries.entrytable, table->info.size);
  }
  close(fd);
}

static void reset_iptables(struct ipt_table_desc* tables, int num_tables,
                           int family, int level)
{
  struct xt_counters counters[XT_MAX_ENTRIES];
  struct ipt_get_entries entries;
  struct ipt_getinfo info;
  socklen_t optlen;
  int fd, i;

  fd = socket(family, SOCK_STREAM, IPPROTO_TCP);
  if (fd == -1) {
    switch (errno) {
    case EAFNOSUPPORT:
    case ENOPROTOOPT:
      return;
    }
    fail("socket(%d, SOCK_STREAM, IPPROTO_TCP)", family);
  }
  for (i = 0; i < num_tables; i++) {
    struct ipt_table_desc* table = &tables[i];
    if (table->info.valid_hooks == 0)
      continue;
    memset(&info, 0, sizeof(info));
    strcpy(info.name, table->name);
    optlen = sizeof(info);
    if (getsockopt(fd, level, IPT_SO_GET_INFO, &info, &optlen))
      fail("getsockopt(IPT_SO_GET_INFO)");
    if (memcmp(&table->info, &info, sizeof(table->info)) == 0) {
      memset(&entries, 0, sizeof(entries));
      strcpy(entries.name, table->name);
      entries.size = table->info.size;
      optlen = sizeof(entries) - sizeof(entries.entrytable) + entries.size;
      if (getsockopt(fd, level, IPT_SO_GET_ENTRIES, &entries, &optlen))
        fail("getsockopt(IPT_SO_GET_ENTRIES)");
      if (memcmp(table->replace.entrytable, entries.entrytable,
                 table->info.size) == 0)
        continue;
    }
    table->replace.num_counters = info.num_entries;
    table->replace.counters = counters;
    optlen = sizeof(table->replace) - sizeof(table->replace.entrytable) +
             table->replace.size;
    if (setsockopt(fd, level, IPT_SO_SET_REPLACE, &table->replace, optlen))
      fail("setsockopt(IPT_SO_SET_REPLACE)");
  }
  close(fd);
}

static void checkpoint_arptables(void)
{
  struct arpt_get_entries entries;
  socklen_t optlen;
  unsigned i;
  int fd;

  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd == -1)
    fail("socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)");
  for (i = 0; i < sizeof(arpt_tables) / sizeof(arpt_tables[0]); i++) {
    struct arpt_table_desc* table = &arpt_tables[i];
    strcpy(table->info.name, table->name);
    strcpy(table->replace.name, table->name);
    optlen = sizeof(table->info);
    if (getsockopt(fd, SOL_IP, ARPT_SO_GET_INFO, &table->info, &optlen)) {
      switch (errno) {
      case EPERM:
      case ENOENT:
      case ENOPROTOOPT:
        continue;
      }
      fail("getsockopt(ARPT_SO_GET_INFO)");
    }
    if (table->info.size > sizeof(table->replace.entrytable))
      fail("table size is too large: %u", table->info.size);
    if (table->info.num_entries > XT_MAX_ENTRIES)
      fail("too many counters: %u", table->info.num_entries);
    memset(&entries, 0, sizeof(entries));
    strcpy(entries.name, table->name);
    entries.size = table->info.size;
    optlen = sizeof(entries) - sizeof(entries.entrytable) + table->info.size;
    if (getsockopt(fd, SOL_IP, ARPT_SO_GET_ENTRIES, &entries, &optlen))
      fail("getsockopt(ARPT_SO_GET_ENTRIES)");
    table->replace.valid_hooks = table->info.valid_hooks;
    table->replace.num_entries = table->info.num_entries;
    table->replace.size = table->info.size;
    memcpy(table->replace.hook_entry, table->info.hook_entry,
           sizeof(table->replace.hook_entry));
    memcpy(table->replace.underflow, table->info.underflow,
           sizeof(table->replace.underflow));
    memcpy(table->replace.entrytable, entries.entrytable, table->info.size);
  }
  close(fd);
}

static void reset_arptables()
{
  struct xt_counters counters[XT_MAX_ENTRIES];
  struct arpt_get_entries entries;
  struct arpt_getinfo info;
  socklen_t optlen;
  unsigned i;
  int fd;

  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd == -1)
    fail("socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)");
  for (i = 0; i < sizeof(arpt_tables) / sizeof(arpt_tables[0]); i++) {
    struct arpt_table_desc* table = &arpt_tables[i];
    if (table->info.valid_hooks == 0)
      continue;
    memset(&info, 0, sizeof(info));
    strcpy(info.name, table->name);
    optlen = sizeof(info);
    if (getsockopt(fd, SOL_IP, ARPT_SO_GET_INFO, &info, &optlen))
      fail("getsockopt(ARPT_SO_GET_INFO)");
    if (memcmp(&table->info, &info, sizeof(table->info)) == 0) {
      memset(&entries, 0, sizeof(entries));
      strcpy(entries.name, table->name);
      entries.size = table->info.size;
      optlen = sizeof(entries) - sizeof(entries.entrytable) + entries.size;
      if (getsockopt(fd, SOL_IP, ARPT_SO_GET_ENTRIES, &entries, &optlen))
        fail("getsockopt(ARPT_SO_GET_ENTRIES)");
      if (memcmp(table->replace.entrytable, entries.entrytable,
                 table->info.size) == 0)
        continue;
    }
    table->replace.num_counters = info.num_entries;
    table->replace.counters = counters;
    optlen = sizeof(table->replace) - sizeof(table->replace.entrytable) +
             table->replace.size;
    if (setsockopt(fd, SOL_IP, ARPT_SO_SET_REPLACE, &table->replace, optlen))
      fail("setsockopt(ARPT_SO_SET_REPLACE)");
  }
  close(fd);
}
#include <linux/if.h>
#include <linux/netfilter_bridge/ebtables.h>

struct ebt_table_desc {
  const char* name;
  struct ebt_replace replace;
  char entrytable[XT_TABLE_SIZE];
};

static struct ebt_table_desc ebt_tables[] = {
    {.name = "filter"}, {.name = "nat"}, {.name = "broute"},
};

static void checkpoint_ebtables(void)
{
  socklen_t optlen;
  unsigned i;
  int fd;

  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd == -1)
    fail("socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)");
  for (i = 0; i < sizeof(ebt_tables) / sizeof(ebt_tables[0]); i++) {
    struct ebt_table_desc* table = &ebt_tables[i];
    strcpy(table->replace.name, table->name);
    optlen = sizeof(table->replace);
    if (getsockopt(fd, SOL_IP, EBT_SO_GET_INIT_INFO, &table->replace,
                   &optlen)) {
      switch (errno) {
      case EPERM:
      case ENOENT:
      case ENOPROTOOPT:
        continue;
      }
      fail("getsockopt(EBT_SO_GET_INIT_INFO)");
    }
    if (table->replace.entries_size > sizeof(table->entrytable))
      fail("table size is too large: %u", table->replace.entries_size);
    table->replace.num_counters = 0;
    table->replace.entries = table->entrytable;
    optlen = sizeof(table->replace) + table->replace.entries_size;
    if (getsockopt(fd, SOL_IP, EBT_SO_GET_INIT_ENTRIES, &table->replace,
                   &optlen))
      fail("getsockopt(EBT_SO_GET_INIT_ENTRIES)");
  }
  close(fd);
}

static void reset_ebtables()
{
  struct ebt_replace replace;
  char entrytable[XT_TABLE_SIZE];
  socklen_t optlen;
  unsigned i, j, h;
  int fd;

  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd == -1)
    fail("socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)");
  for (i = 0; i < sizeof(ebt_tables) / sizeof(ebt_tables[0]); i++) {
    struct ebt_table_desc* table = &ebt_tables[i];
    if (table->replace.valid_hooks == 0)
      continue;
    memset(&replace, 0, sizeof(replace));
    strcpy(replace.name, table->name);
    optlen = sizeof(replace);
    if (getsockopt(fd, SOL_IP, EBT_SO_GET_INFO, &replace, &optlen))
      fail("getsockopt(EBT_SO_GET_INFO)");
    replace.num_counters = 0;
    table->replace.entries = 0;
    for (h = 0; h < NF_BR_NUMHOOKS; h++)
      table->replace.hook_entry[h] = 0;
    if (memcmp(&table->replace, &replace, sizeof(table->replace)) == 0) {
      memset(&entrytable, 0, sizeof(entrytable));
      replace.entries = entrytable;
      optlen = sizeof(replace) + replace.entries_size;
      if (getsockopt(fd, SOL_IP, EBT_SO_GET_ENTRIES, &replace, &optlen))
        fail("getsockopt(EBT_SO_GET_ENTRIES)");
      if (memcmp(table->entrytable, entrytable, replace.entries_size) == 0)
        continue;
    }
    for (j = 0, h = 0; h < NF_BR_NUMHOOKS; h++) {
      if (table->replace.valid_hooks & (1 << h)) {
        table->replace.hook_entry[h] =
            (struct ebt_entries*)table->entrytable + j;
        j++;
      }
    }
    table->replace.entries = table->entrytable;
    optlen = sizeof(table->replace) + table->replace.entries_size;
    if (setsockopt(fd, SOL_IP, EBT_SO_SET_ENTRIES, &table->replace, optlen))
      fail("setsockopt(EBT_SO_SET_ENTRIES)");
  }
  close(fd);
}

static void checkpoint_net_namespace(void)
{
  checkpoint_ebtables();
  checkpoint_arptables();
  checkpoint_iptables(ipv4_tables, sizeof(ipv4_tables) / sizeof(ipv4_tables[0]),
                      AF_INET, SOL_IP);
  checkpoint_iptables(ipv6_tables, sizeof(ipv6_tables) / sizeof(ipv6_tables[0]),
                      AF_INET6, SOL_IPV6);
}

static void reset_net_namespace(void)
{
  reset_ebtables();
  reset_arptables();
  reset_iptables(ipv4_tables, sizeof(ipv4_tables) / sizeof(ipv4_tables[0]),
                 AF_INET, SOL_IP);
  reset_iptables(ipv6_tables, sizeof(ipv6_tables) / sizeof(ipv6_tables[0]),
                 AF_INET6, SOL_IPV6);
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
  checkpoint_net_namespace();
  int iter;
  for (iter = 0;; iter++) {
    char cwdbuf[32];
    sprintf(cwdbuf, "./%d", iter);
    if (mkdir(cwdbuf, 0777))
      fail("failed to mkdir");
    char buf[64];
    snprintf(buf, sizeof(buf), "/dev/loop%llu", procid);
    int loopfd = open(buf, O_RDWR);
    if (loopfd != -1) {
      ioctl(loopfd, LOOP_CLR_FD, 0);
      close(loopfd);
    }
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
    reset_net_namespace();
  }
}

unsigned long long procid;
void execute_one()
{
  memcpy((void*)0x20000000, "f2fs", 5);
  memcpy((void*)0x20000100, "./file0", 8);
  *(uint64_t*)0x20000200 = 0x20010000;
  *(uint64_t*)0x20000208 = 0;
  *(uint64_t*)0x20000210 = 0x400;
  *(uint64_t*)0x20000218 = 0x20010100;
  *(uint64_t*)0x20000220 = 0;
  *(uint64_t*)0x20000228 = 0x860;
  *(uint64_t*)0x20000230 = 0x20010200;
  *(uint64_t*)0x20000238 = 0;
  *(uint64_t*)0x20000240 = 0xa80;
  *(uint64_t*)0x20000248 = 0x20010300;
  *(uint64_t*)0x20000250 = 0;
  *(uint64_t*)0x20000258 = 0xb80;
  *(uint64_t*)0x20000260 = 0x20010400;
  memcpy((void*)0x20010400,
         "\x10\x20\xf5\xf2\x01\x00\x07\x00\x09\x00\x00\x00\x03\x00\x00\x00\x0c"
         "\x00\x00\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x28\x00\x00\x00\x00\x00\x00\x0c\x00\x00\x00\x13\x00\x00"
         "\x00\x02\x00\x00\x00\x02\x00\x00\x00\x02\x00\x00\x00\x01\x00\x00\x00"
         "\x0c\x00\x00\x00\x00\x02\x00\x00\x00\x02\x00\x00\x00\x06\x00\x00\x00"
         "\x0a\x00\x00\x00\x0e\x00\x00\x00\x10\x00\x00\x03\x00\x00\x00\x01\x00"
         "\x00\x00\x02\x00\x00\x00\x5b\xe8\xf8\x91\x37\x11\x43\x91\xb6\x1d\x22"
         "\x28\x94\x8a\xeb\x15\x73\x00\x79\x00\x7a\x00\x30\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00",
         160);
  *(uint64_t*)0x20000268 = 0xa0;
  *(uint64_t*)0x20000270 = 0x1400;
  *(uint64_t*)0x20000278 = 0x20010500;
  *(uint64_t*)0x20000280 = 0;
  *(uint64_t*)0x20000288 = 0x1860;
  *(uint64_t*)0x20000290 = 0x20010600;
  *(uint64_t*)0x20000298 = 0;
  *(uint64_t*)0x200002a0 = 0x1a80;
  *(uint64_t*)0x200002a8 = 0x20010700;
  *(uint64_t*)0x200002b0 = 0;
  *(uint64_t*)0x200002b8 = 0x1b80;
  *(uint64_t*)0x200002c0 = 0x20010800;
  memcpy((void*)0x20010800,
         "\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x0c"
         "\x00\x00\x00\x00\x00\x00\x00\x0a\x00\x00\x00\x0b\x00\x00\x00\x06\x00"
         "\x00\x00\x0b\x00\x00\x00\x0a\x00\x00\x00\x09\x00\x00\x00\xff\xff\xff"
         "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
         "\x0d\x00\x0b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08"
         "\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff"
         "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x09\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x45\x00\x00\x00"
         "\x06\x00\x00\x00\x01\x00\x00\x00\x0b\x00\x00\x00\x0b\x00\x00\x00\x0e"
         "\x00\x00\x00\x40\x00\x00\x00\x40\x00\x00\x00\xfc\x0f\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00",
         192);
  *(uint64_t*)0x200002c8 = 0xc0;
  *(uint64_t*)0x200002d0 = 0x200000;
  *(uint64_t*)0x200002d8 = 0x20010900;
  memcpy((void*)0x20010900,
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7d\x96\x92\x6f\x0b\x00"
         "\x03\x00\x00\x00\x00\x03\x00\x00\x00\x0c\x26\x00\x00\x04\x00\x00\x00"
         "\x00\x04\x00\x00\x00\x01\x24\x00\x00\x05\x00\x00\x00\x00\x05\x00\x00"
         "\x00\x04\x26\x00\x00\x06\x00\x00\x00\x00\x06\x00\x00\x00\x03\x24\x00"
         "\x00\x07\x00\x00\x00\x00\x07\x00\x00\x00\x04\x24\x00\x00\x08\x00\x00"
         "\x00\x00\x08\x00\x00\x00\x05\x24\x00\x00\x09\x00\x00\x00\x00\x09\x00"
         "\x00\x00\x06\x24\x00\x00\x0a\x00\x00\x00\x00\x0a\x00\x00\x00\x07\x24"
         "\x00\x00\x0b\x00\x00\x00\x00\x0b\x00\x00\x00\x0b\x26\x00\x00\x0c\x00"
         "\x00\x00\x00\x0c\x00\x00\x00\x08\x24\x00\x00\x0d\x00\x00\x00\x00\x0d"
         "\x00\x00\x00\x0a\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00",
         192);
  *(uint64_t*)0x200002e0 = 0xc0;
  *(uint64_t*)0x200002e8 = 0x200fe0;
  *(uint64_t*)0x200002f0 = 0x20010a00;
  *(uint64_t*)0x200002f8 = 0;
  *(uint64_t*)0x20000300 = 0x2011e0;
  *(uint64_t*)0x20000308 = 0x20010b00;
  *(uint64_t*)0x20000310 = 0;
  *(uint64_t*)0x20000318 = 0x201240;
  *(uint64_t*)0x20000320 = 0x20010c00;
  *(uint64_t*)0x20000328 = 0;
  *(uint64_t*)0x20000330 = 0x201280;
  *(uint64_t*)0x20000338 = 0x20010d00;
  *(uint64_t*)0x20000340 = 0;
  *(uint64_t*)0x20000348 = 0x2012e0;
  *(uint64_t*)0x20000350 = 0x20010e00;
  *(uint64_t*)0x20000358 = 0;
  *(uint64_t*)0x20000360 = 0x201320;
  *(uint64_t*)0x20000368 = 0x20010f00;
  *(uint64_t*)0x20000370 = 0;
  *(uint64_t*)0x20000378 = 5;
  *(uint64_t*)0x20000380 = 0x20011000;
  *(uint64_t*)0x20000388 = 0;
  *(uint64_t*)0x20000390 = 0x2013e0;
  *(uint64_t*)0x20000398 = 0x20011100;
  *(uint64_t*)0x200003a0 = 0;
  *(uint64_t*)0x200003a8 = 0x202000;
  *(uint64_t*)0x200003b0 = 0x20011200;
  *(uint64_t*)0x200003b8 = 0;
  *(uint64_t*)0x200003c0 = 0x202e00;
  *(uint64_t*)0x200003c8 = 0x20011300;
  *(uint64_t*)0x200003d0 = 0;
  *(uint64_t*)0x200003d8 = 0x202fe0;
  *(uint64_t*)0x200003e0 = 0x20011400;
  *(uint64_t*)0x200003e8 = 0;
  *(uint64_t*)0x200003f0 = 0x203fe0;
  *(uint64_t*)0x200003f8 = 0x20011500;
  memcpy((void*)0x20011500,
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x0d\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x0c\x00\x00"
         "\x00\x00\x00\x00\x00\x0a\x00\x00\x00\x0b\x00\x00\x00\x06\x00\x00\x00"
         "\x0b\x00\x00\x00\x0a\x00\x00\x00\x09\x00\x00\x00\xff\xff\xff\xff\xff"
         "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x0d\x00"
         "\x0b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00"
         "\x00\x01\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff"
         "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x09\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x45\x00\x00\x00\x06\x00"
         "\x00\x00\x01\x00\x00\x00\x0b\x00\x00\x00\x0b\x00\x00\x00\x0e\x00\x00"
         "\x00\x40\x00\x00\x00\x40\x00\x00\x00\xfc\x0f\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00",
         224);
  *(uint64_t*)0x20000400 = 0xe0;
  *(uint64_t*)0x20000408 = 0x204fe0;
  *(uint64_t*)0x20000410 = 0x20011600;
  memcpy((void*)0x20011600, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                            "\x00\x00\x00\x00\x7d\x96\x92\x6f",
         32);
  *(uint64_t*)0x20000418 = 0x20;
  *(uint64_t*)0x20000420 = 0x205fe0;
  *(uint64_t*)0x20000428 = 0x20011700;
  *(uint64_t*)0x20000430 = 0;
  *(uint64_t*)0x20000438 = 0x400000;
  *(uint64_t*)0x20000440 = 0x20011800;
  *(uint64_t*)0x20000448 = 0;
  *(uint64_t*)0x20000450 = 0x400fe0;
  *(uint64_t*)0x20000458 = 0x20011900;
  *(uint64_t*)0x20000460 = 0;
  *(uint64_t*)0x20000468 = 0x4011e0;
  *(uint64_t*)0x20000470 = 0x20011a00;
  *(uint64_t*)0x20000478 = 0;
  *(uint64_t*)0x20000480 = 0x401240;
  *(uint64_t*)0x20000488 = 0x20011b00;
  *(uint64_t*)0x20000490 = 0;
  *(uint64_t*)0x20000498 = 0x401280;
  *(uint64_t*)0x200004a0 = 0x20011c00;
  *(uint64_t*)0x200004a8 = 0;
  *(uint64_t*)0x200004b0 = 0x4012e0;
  *(uint64_t*)0x200004b8 = 0x20011d00;
  *(uint64_t*)0x200004c0 = 0;
  *(uint64_t*)0x200004c8 = 0x401320;
  *(uint64_t*)0x200004d0 = 0x20011e00;
  *(uint64_t*)0x200004d8 = 0;
  *(uint64_t*)0x200004e0 = 0x401380;
  *(uint64_t*)0x200004e8 = 0x20011f00;
  *(uint64_t*)0x200004f0 = 0;
  *(uint64_t*)0x200004f8 = 0x4013e0;
  *(uint64_t*)0x20000500 = 0x20012000;
  *(uint64_t*)0x20000508 = 0;
  *(uint64_t*)0x20000510 = 0x402000;
  *(uint64_t*)0x20000518 = 0x20012100;
  *(uint64_t*)0x20000520 = 0;
  *(uint64_t*)0x20000528 = 0x402e00;
  *(uint64_t*)0x20000530 = 0x20012200;
  *(uint64_t*)0x20000538 = 0;
  *(uint64_t*)0x20000540 = 0x402fe0;
  *(uint64_t*)0x20000548 = 0x20012300;
  *(uint64_t*)0x20000550 = 0;
  *(uint64_t*)0x20000558 = 0x403fe0;
  *(uint64_t*)0x20000560 = 0x20012400;
  *(uint64_t*)0x20000568 = 0;
  *(uint64_t*)0x20000570 = 0x404fe0;
  *(uint64_t*)0x20000578 = 0x20012500;
  *(uint64_t*)0x20000580 = 0;
  *(uint64_t*)0x20000588 = 0x405fe0;
  *(uint64_t*)0x20000590 = 0x20012600;
  *(uint64_t*)0x20000598 = 0;
  *(uint64_t*)0x200005a0 = 0xa00000;
  *(uint64_t*)0x200005a8 = 0x20012700;
  *(uint64_t*)0x200005b0 = 0;
  *(uint64_t*)0x200005b8 = 0x2000000;
  *(uint64_t*)0x200005c0 = 0x20012800;
  *(uint64_t*)0x200005c8 = 0;
  *(uint64_t*)0x200005d0 = 0x2000940;
  *(uint64_t*)0x200005d8 = 0x20012900;
  *(uint64_t*)0x200005e0 = 0;
  *(uint64_t*)0x200005e8 = 0x2001000;
  *(uint64_t*)0x200005f0 = 0x20012a00;
  *(uint64_t*)0x200005f8 = 0;
  *(uint64_t*)0x20000600 = 0x2001940;
  *(uint64_t*)0x20000608 = 0x20012b00;
  *(uint64_t*)0x20000610 = 0;
  *(uint64_t*)0x20000618 = 0x2002000;
  *(uint64_t*)0x20000620 = 0x20012c00;
  *(uint64_t*)0x20000628 = 0;
  *(uint64_t*)0x20000630 = 0x2002940;
  *(uint64_t*)0x20000638 = 0x20012d00;
  *(uint64_t*)0x20000640 = 0;
  *(uint64_t*)0x20000648 = 0x2003000;
  *(uint64_t*)0x20000650 = 0x20012e00;
  *(uint64_t*)0x20000658 = 0;
  *(uint64_t*)0x20000660 = 0x2003940;
  *(uint64_t*)0x20000668 = 0x20012f00;
  *(uint64_t*)0x20000670 = 0;
  *(uint64_t*)0x20000678 = 0x2004000;
  *(uint64_t*)0x20000680 = 0x20013000;
  *(uint64_t*)0x20000688 = 0;
  *(uint64_t*)0x20000690 = 0x2004940;
  *(uint64_t*)0x20000698 = 0x20013100;
  *(uint64_t*)0x200006a0 = 0;
  *(uint64_t*)0x200006a8 = 0x2005000;
  *(uint64_t*)0x200006b0 = 0x20013200;
  *(uint64_t*)0x200006b8 = 0;
  *(uint64_t*)0x200006c0 = 0x2005940;
  *(uint64_t*)0x200006c8 = 0x20013300;
  *(uint64_t*)0x200006d0 = 0;
  *(uint64_t*)0x200006d8 = 0x2006000;
  *(uint64_t*)0x200006e0 = 0x20013400;
  *(uint64_t*)0x200006e8 = 0;
  *(uint64_t*)0x200006f0 = 0x2006940;
  *(uint64_t*)0x200006f8 = 0x20013500;
  *(uint64_t*)0x20000700 = 0;
  *(uint64_t*)0x20000708 = 0x2007000;
  *(uint64_t*)0x20000710 = 0x20013600;
  *(uint64_t*)0x20000718 = 0;
  *(uint64_t*)0x20000720 = 0x2007940;
  *(uint64_t*)0x20000728 = 0x20013700;
  *(uint64_t*)0x20000730 = 0;
  *(uint64_t*)0x20000738 = 0x2008000;
  *(uint64_t*)0x20000740 = 0x20013800;
  *(uint64_t*)0x20000748 = 0;
  *(uint64_t*)0x20000750 = 0x2008940;
  *(uint64_t*)0x20000758 = 0x20013900;
  *(uint64_t*)0x20000760 = 0;
  *(uint64_t*)0x20000768 = 0x2400000;
  *(uint64_t*)0x20000770 = 0x20013a00;
  *(uint64_t*)0x20000778 = 0;
  *(uint64_t*)0x20000780 = 0x2400160;
  *(uint64_t*)0x20000788 = 0x20013b00;
  *(uint64_t*)0x20000790 = 0;
  *(uint64_t*)0x20000798 = 0x2400fe0;
  *(uint64_t*)0x200007a0 = 0x20013c00;
  *(uint64_t*)0x200007a8 = 0;
  *(uint64_t*)0x200007b0 = 0x2401160;
  *(uint64_t*)0x200007b8 = 0x20013d00;
  *(uint64_t*)0x200007c0 = 0;
  *(uint64_t*)0x200007c8 = 0x2401fe0;
  *(uint64_t*)0x200007d0 = 0x20013e00;
  *(uint64_t*)0x200007d8 = 0;
  *(uint64_t*)0x200007e0 = 0x2402160;
  *(uint64_t*)0x200007e8 = 0x20013f00;
  *(uint64_t*)0x200007f0 = 0;
  *(uint64_t*)0x200007f8 = 0x2402fe0;
  *(uint64_t*)0x20000800 = 0x20014000;
  *(uint64_t*)0x20000808 = 0;
  *(uint64_t*)0x20000810 = 0x2403160;
  *(uint64_t*)0x20000818 = 0x20014100;
  *(uint64_t*)0x20000820 = 0;
  *(uint64_t*)0x20000828 = 0x2403fe0;
  *(uint64_t*)0x20000830 = 0x20014200;
  *(uint64_t*)0x20000838 = 0;
  *(uint64_t*)0x20000840 = 0x2404fe0;
  *(uint64_t*)0x20000848 = 0x20014300;
  *(uint64_t*)0x20000850 = 0;
  *(uint64_t*)0x20000858 = 0x2405fe0;
  *(uint64_t*)0x20000860 = 0x20014400;
  *(uint64_t*)0x20000868 = 0;
  *(uint64_t*)0x20000870 = 0x2406fe0;
  *(uint64_t*)0x20000878 = 0x20014500;
  *(uint64_t*)0x20000880 = 0;
  *(uint64_t*)0x20000888 = 0x2407fe0;
  *(uint64_t*)0x20000890 = 0x20014600;
  *(uint64_t*)0x20000898 = 0;
  *(uint64_t*)0x200008a0 = 0x2408fe0;
  *(uint64_t*)0x200008a8 = 0x20014700;
  *(uint64_t*)0x200008b0 = 0;
  *(uint64_t*)0x200008b8 = 0x2409160;
  *(uint64_t*)0x200008c0 = 0x20014800;
  *(uint64_t*)0x200008c8 = 0;
  *(uint64_t*)0x200008d0 = 0x2409fe0;
  *(uint64_t*)0x200008d8 = 0x20014900;
  *(uint64_t*)0x200008e0 = 0;
  *(uint64_t*)0x200008e8 = 0x240a160;
  *(uint64_t*)0x200008f0 = 0x20014a00;
  *(uint64_t*)0x200008f8 = 0;
  *(uint64_t*)0x20000900 = 0x240afe0;
  *(uint64_t*)0x20000908 = 0x20014b00;
  *(uint64_t*)0x20000910 = 0;
  *(uint64_t*)0x20000918 = 0x2600000;
  *(uint64_t*)0x20000920 = 0x20014c00;
  *(uint64_t*)0x20000928 = 0;
  *(uint64_t*)0x20000930 = 0x2600160;
  *(uint64_t*)0x20000938 = 0x20014d00;
  *(uint64_t*)0x20000940 = 0;
  *(uint64_t*)0x20000948 = 0x2600fe0;
  *(uint64_t*)0x20000950 = 0x20014e00;
  *(uint64_t*)0x20000958 = 0;
  *(uint64_t*)0x20000960 = 0x2601160;
  *(uint64_t*)0x20000968 = 0x20014f00;
  *(uint64_t*)0x20000970 = 0;
  *(uint64_t*)0x20000978 = 0x2601fe0;
  *(uint64_t*)0x20000980 = 0x20015000;
  *(uint64_t*)0x20000988 = 0;
  *(uint64_t*)0x20000990 = 0x2602160;
  *(uint64_t*)0x20000998 = 0x20015100;
  *(uint64_t*)0x200009a0 = 0;
  *(uint64_t*)0x200009a8 = 0x2602fe0;
  *(uint64_t*)0x200009b0 = 0x20015200;
  *(uint64_t*)0x200009b8 = 0;
  *(uint64_t*)0x200009c0 = 0x2603160;
  *(uint64_t*)0x200009c8 = 0x20015300;
  *(uint64_t*)0x200009d0 = 0;
  *(uint64_t*)0x200009d8 = 0x2603940;
  *(uint64_t*)0x200009e0 = 0x20015400;
  *(uint64_t*)0x200009e8 = 0;
  *(uint64_t*)0x200009f0 = 0x2603fe0;
  *(uint64_t*)0x200009f8 = 0x20015500;
  *(uint64_t*)0x20000a00 = 0;
  *(uint64_t*)0x20000a08 = 0x2604160;
  *(uint64_t*)0x20000a10 = 0x20015600;
  *(uint64_t*)0x20000a18 = 0;
  *(uint64_t*)0x20000a20 = 0x2604940;
  *(uint64_t*)0x20000a28 = 0x20015700;
  *(uint64_t*)0x20000a30 = 0;
  *(uint64_t*)0x20000a38 = 0x2604fe0;
  *(uint64_t*)0x20000a40 = 0x20015800;
  *(uint64_t*)0x20000a48 = 0;
  *(uint64_t*)0x20000a50 = 0x2605160;
  *(uint64_t*)0x20000a58 = 0x20015900;
  *(uint64_t*)0x20000a60 = 0;
  *(uint64_t*)0x20000a68 = 0x2605fe0;
  *(uint64_t*)0x20000a70 = 0x20015a00;
  *(uint64_t*)0x20000a78 = 0;
  *(uint64_t*)0x20000a80 = 0x2606160;
  *(uint64_t*)0x20000a88 = 0x20015b00;
  *(uint64_t*)0x20000a90 = 0;
  *(uint64_t*)0x20000a98 = 0x2606fe0;
  *(uint64_t*)0x20000aa0 = 0x20015c00;
  *(uint64_t*)0x20000aa8 = 0;
  *(uint64_t*)0x20000ab0 = 0x2607160;
  *(uint64_t*)0x20000ab8 = 0x20015d00;
  *(uint64_t*)0x20000ac0 = 0;
  *(uint64_t*)0x20000ac8 = 0x2607fe0;
  *(uint64_t*)0x20000ad0 = 0x20015e00;
  *(uint64_t*)0x20000ad8 = 0;
  *(uint64_t*)0x20000ae0 = 0x2608160;
  *(uint64_t*)0x20000ae8 = 0x20015f00;
  *(uint64_t*)0x20000af0 = 0;
  *(uint64_t*)0x20000af8 = 0x2608fe0;
  *(uint64_t*)0x20000b00 = 0x20016000;
  *(uint64_t*)0x20000b08 = 0;
  *(uint64_t*)0x20000b10 = 0x2609160;
  *(uint64_t*)0x20000b18 = 0x20016100;
  *(uint64_t*)0x20000b20 = 0;
  *(uint64_t*)0x20000b28 = 0x2609fe0;
  *(uint64_t*)0x20000b30 = 0x20016200;
  *(uint64_t*)0x20000b38 = 0;
  *(uint64_t*)0x20000b40 = 0x260a160;
  *(uint64_t*)0x20000b48 = 0x20016300;
  *(uint64_t*)0x20000b50 = 0;
  *(uint64_t*)0x20000b58 = 0x260a940;
  *(uint64_t*)0x20000b60 = 0x20016400;
  *(uint64_t*)0x20000b68 = 0;
  *(uint64_t*)0x20000b70 = 0x260afe0;
  *(uint64_t*)0x20000b78 = 0x20016500;
  *(uint64_t*)0x20000b80 = 0;
  *(uint64_t*)0x20000b88 = 0x260b160;
  *(uint64_t*)0x20000b90 = 0x20016600;
  *(uint64_t*)0x20000b98 = 0;
  *(uint64_t*)0x20000ba0 = 0x260b940;
  *(uint64_t*)0x20000ba8 = 0x20016700;
  memcpy((void*)0x20016700,
         "\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x0b\x00\x00\x00\x00"
         "\x00\x00\x00\x09\x00\x00\x00\x29\xdb\xda\x7f\x0c\x26\x00\x00\xed\x41"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x10\x00"
         "\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x78\xa0\xd8\x5a"
         "\x00\x00\x00\x00\x78\xa0\xd8\x5a\x00\x00\x00\x00\x78\xa0\xd8\x5a\x00"
         "\x00\x00\x00\x69\x6c\xcd\x30\x9b\x56\x00\x37\x9b\x56\x00\x37\x00\x00"
         "\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00",
         128);
  *(uint64_t*)0x20000bb0 = 0x80;
  *(uint64_t*)0x20000bb8 = 0x260bfe0;
  *(uint64_t*)0x20000bc0 = 0x20016800;
  *(uint64_t*)0x20000bc8 = 0;
  *(uint64_t*)0x20000bd0 = 0x260c160;
  *(uint64_t*)0x20000bd8 = 0x20016900;
  memcpy((void*)0x20016900, "\x00\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00"
                            "\x03\x00\x00\x00\x00\x00\x00\x00\x0a\x00\x00\x00"
                            "\x5a\xab\xd9\x50\x0d\x26\x00\x00",
         32);
  *(uint64_t*)0x20000be0 = 0x20;
  *(uint64_t*)0x20000be8 = 0x260cfe0;
  *(uint8_t*)0x20016a00 = 0;
  syz_mount_image(0x20000000, 0x20000100, 0x2800000, 0x6a, 0x20000200, 0,
                  0x20016a00);
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