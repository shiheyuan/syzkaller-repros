// general protection fault in skb_segment
// https://syzkaller.appspot.com/bug?id=acb8c86b04c5631736c851bb750e527350d76164
// status:fixed
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <fcntl.h>
#include <linux/capability.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/wait.h>
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

struct csum_inet {
  uint32_t acc;
};

static void csum_inet_init(struct csum_inet* csum)
{
  csum->acc = 0;
}

static void csum_inet_update(struct csum_inet* csum, const uint8_t* data,
                             size_t length)
{
  if (length == 0)
    return;

  size_t i;
  for (i = 0; i < length - 1; i += 2)
    csum->acc += *(uint16_t*)&data[i];

  if (length & 1)
    csum->acc += (uint16_t)data[length - 1];

  while (csum->acc > 0xffff)
    csum->acc = (csum->acc & 0xffff) + (csum->acc >> 16);
}

static uint16_t csum_inet_digest(struct csum_inet* csum)
{
  return ~csum->acc;
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

static uintptr_t syz_open_dev(uintptr_t a0, uintptr_t a1, uintptr_t a2)
{
  if (a0 == 0xc || a0 == 0xb) {
    char buf[128];
    sprintf(buf, "/dev/%s/%d:%d", a0 == 0xc ? "char" : "block", (uint8_t)a1,
            (uint8_t)a2);
    return open(buf, O_RDWR, 0);
  } else {
    char buf[1024];
    char* hash;
    strncpy(buf, (char*)a0, sizeof(buf));
    buf[sizeof(buf) - 1] = 0;
    while ((hash = strchr(buf, '#'))) {
      *hash = '0' + (char)(a1 % 10);
      a1 /= 10;
    }
    return open(buf, a2, 0);
  }
}

static void loop();

static void sandbox_common()
{
  prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
  setpgrp();
  setsid();

  struct rlimit rlim;
  rlim.rlim_cur = rlim.rlim_max = 128 << 20;
  setrlimit(RLIMIT_AS, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 8 << 20;
  setrlimit(RLIMIT_MEMLOCK, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 1 << 20;
  setrlimit(RLIMIT_FSIZE, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 1 << 20;
  setrlimit(RLIMIT_STACK, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 0;
  setrlimit(RLIMIT_CORE, &rlim);

#define CLONE_NEWCGROUP 0x02000000

  unshare(CLONE_NEWNS);
  unshare(CLONE_NEWIPC);
  unshare(CLONE_NEWCGROUP);
  unshare(CLONE_NEWNET);
  unshare(CLONE_NEWUTS);
  unshare(CLONE_SYSVSEM);
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

static int real_uid;
static int real_gid;
__attribute__((aligned(64 << 10))) static char sandbox_stack[1 << 20];

static int namespace_sandbox_proc(void* arg)
{
  sandbox_common();

  write_file("/proc/self/setgroups", "deny");
  if (!write_file("/proc/self/uid_map", "0 %d 1\n", real_uid))
    fail("write of /proc/self/uid_map failed");
  if (!write_file("/proc/self/gid_map", "0 %d 1\n", real_gid))
    fail("write of /proc/self/gid_map failed");

  if (mkdir("./syz-tmp", 0777))
    fail("mkdir(syz-tmp) failed");
  if (mount("", "./syz-tmp", "tmpfs", 0, NULL))
    fail("mount(tmpfs) failed");
  if (mkdir("./syz-tmp/newroot", 0777))
    fail("mkdir failed");
  if (mkdir("./syz-tmp/newroot/dev", 0700))
    fail("mkdir failed");
  if (mount("/dev", "./syz-tmp/newroot/dev", NULL,
            MS_BIND | MS_REC | MS_PRIVATE, NULL))
    fail("mount(dev) failed");
  if (mkdir("./syz-tmp/newroot/proc", 0700))
    fail("mkdir failed");
  if (mount(NULL, "./syz-tmp/newroot/proc", "proc", 0, NULL))
    fail("mount(proc) failed");
  if (mkdir("./syz-tmp/pivot", 0777))
    fail("mkdir failed");
  if (syscall(SYS_pivot_root, "./syz-tmp", "./syz-tmp/pivot")) {
    if (chdir("./syz-tmp"))
      fail("chdir failed");
  } else {
    if (chdir("/"))
      fail("chdir failed");
    if (umount2("./pivot", MNT_DETACH))
      fail("umount failed");
  }
  if (chroot("./newroot"))
    fail("chroot failed");
  if (chdir("/"))
    fail("chdir failed");

  struct __user_cap_header_struct cap_hdr = {};
  struct __user_cap_data_struct cap_data[2] = {};
  cap_hdr.version = _LINUX_CAPABILITY_VERSION_3;
  cap_hdr.pid = getpid();
  if (syscall(SYS_capget, &cap_hdr, &cap_data))
    fail("capget failed");
  cap_data[0].effective &= ~(1 << CAP_SYS_PTRACE);
  cap_data[0].permitted &= ~(1 << CAP_SYS_PTRACE);
  cap_data[0].inheritable &= ~(1 << CAP_SYS_PTRACE);
  if (syscall(SYS_capset, &cap_hdr, &cap_data))
    fail("capset failed");

  loop();
  doexit(1);
}

static int do_sandbox_namespace(int executor_pid, bool enable_tun)
{
  int pid;

  real_uid = getuid();
  real_gid = getgid();
  mprotect(sandbox_stack, 4096, PROT_NONE);
  pid =
      clone(namespace_sandbox_proc, &sandbox_stack[sizeof(sandbox_stack) - 64],
            CLONE_NEWUSER | CLONE_NEWPID, NULL);
  if (pid < 0)
    fail("sandbox clone failed");
  return pid;
}

long r[5];
void loop()
{
  memset(r, -1, sizeof(r));
  syscall(__NR_mmap, 0x20000000, 0xfff000, 3, 0x32, -1, 0);
  r[0] = syscall(__NR_socket, 2, 1, 0);
  memcpy((void*)0x20c55000, "/dev/net/tun", 13);
  r[1] = syz_open_dev(0x20c55000, 0, 0);
  r[2] = syscall(__NR_fcntl, r[1], 0, r[1]);
  r[3] = syscall(__NR_socket, 0x10002000000011, 0x8000000080003, 8);
  *(uint8_t*)0x20927fd8 = 0x73;
  *(uint8_t*)0x20927fd9 = 0x79;
  *(uint8_t*)0x20927fda = 0x7a;
  *(uint8_t*)0x20927fdb = 0x30;
  *(uint8_t*)0x20927fdc = 0;
  *(uint32_t*)0x20927fe8 = 5;
  *(uint32_t*)0x20927fec = 0;
  *(uint64_t*)0x20927ff0 = 0x20c15000;
  *(uint32_t*)0x20c15000 = 0;
  *(uint32_t*)0x20c15004 = 0;
  *(uint16_t*)0x20c15008 = 0;
  syscall(__NR_ioctl, r[1], 0x400454ca, 0x20927fd8);
  *(uint8_t*)0x2062ffe0 = 0x73;
  *(uint8_t*)0x2062ffe1 = 0x79;
  *(uint8_t*)0x2062ffe2 = 0x7a;
  *(uint8_t*)0x2062ffe3 = 0x30;
  *(uint8_t*)0x2062ffe4 = 0;
  *(uint16_t*)0x2062fff0 = 0x301;
  syscall(__NR_ioctl, r[0], 0x8914, 0x2062ffe0);
  *(uint32_t*)0x20113000 = 0x1000;
  syscall(__NR_setsockopt, r[3], 0x107, 0xf, 0x20113000, 4);
  *(uint8_t*)0x20b13000 = 0x73;
  *(uint8_t*)0x20b13001 = 0x79;
  *(uint8_t*)0x20b13002 = 0x7a;
  *(uint8_t*)0x20b13003 = 0x30;
  *(uint8_t*)0x20b13004 = 0;
  *(uint32_t*)0x20b13010 = 0;
  *(uint8_t*)0x20b13014 = 0;
  *(uint8_t*)0x20b13015 = 0;
  *(uint8_t*)0x20b13016 = 0;
  *(uint8_t*)0x20b13017 = 0;
  *(uint8_t*)0x20b13018 = 0;
  *(uint8_t*)0x20b13019 = 0;
  *(uint8_t*)0x20b1301a = 0;
  *(uint8_t*)0x20b1301b = 0;
  *(uint8_t*)0x20b1301c = 0;
  *(uint8_t*)0x20b1301d = 0;
  *(uint8_t*)0x20b1301e = 0;
  *(uint8_t*)0x20b1301f = 0;
  *(uint8_t*)0x20b13020 = 0;
  *(uint8_t*)0x20b13021 = 0;
  *(uint8_t*)0x20b13022 = 0;
  *(uint8_t*)0x20b13023 = 0;
  *(uint8_t*)0x20b13024 = 0;
  *(uint8_t*)0x20b13025 = 0;
  *(uint8_t*)0x20b13026 = 0;
  *(uint8_t*)0x20b13027 = 0;
  if (syscall(__NR_ioctl, r[3], 0x8933, 0x20b13000) != -1)
    r[4] = *(uint32_t*)0x20b13010;
  *(uint16_t*)0x20c85000 = 0x11;
  *(uint16_t*)0x20c85002 = htobe16(0);
  *(uint32_t*)0x20c85004 = r[4];
  *(uint16_t*)0x20c85008 = 1;
  *(uint8_t*)0x20c8500a = 0;
  *(uint8_t*)0x20c8500b = 6;
  *(uint8_t*)0x20c8500c = 0xaa;
  *(uint8_t*)0x20c8500d = 0xaa;
  *(uint8_t*)0x20c8500e = 0xaa;
  *(uint8_t*)0x20c8500f = 0xaa;
  *(uint8_t*)0x20c85010 = 0xaa;
  *(uint8_t*)0x20c85011 = 0;
  *(uint8_t*)0x20c85012 = 0;
  *(uint8_t*)0x20c85013 = 0;
  syscall(__NR_bind, r[3], 0x20c85000, 0x14);
  syscall(__NR_dup2, r[3], r[2]);
  *(uint8_t*)0x20634000 = 0x11;
  *(uint8_t*)0x20634001 = 4;
  *(uint16_t*)0x20634002 = 0;
  *(uint16_t*)0x20634004 = 3;
  *(uint16_t*)0x20634006 = 0;
  *(uint16_t*)0x20634008 = 0;
  STORE_BY_BITMASK(uint8_t, 0x2063400a, 5, 0, 4);
  STORE_BY_BITMASK(uint8_t, 0x2063400a, 4, 4, 4);
  STORE_BY_BITMASK(uint8_t, 0x2063400b, 0, 0, 2);
  STORE_BY_BITMASK(uint8_t, 0x2063400b, 0, 2, 6);
  *(uint16_t*)0x2063400c = htobe16(0x20);
  *(uint16_t*)0x2063400e = htobe16(0x64);
  *(uint16_t*)0x20634010 = htobe16(0);
  *(uint8_t*)0x20634012 = 0;
  *(uint8_t*)0x20634013 = 0x84;
  *(uint16_t*)0x20634014 = 0;
  *(uint32_t*)0x20634016 = htobe32(0);
  *(uint32_t*)0x2063401a = htobe32(0x7f000001);
  *(uint16_t*)0x2063401e = htobe16(0x4e20);
  *(uint16_t*)0x20634020 = htobe16(0x4e20);
  *(uint16_t*)0x20634022 = htobe16(0xc);
  *(uint16_t*)0x20634024 = 0;
  memcpy((void*)0x20634026, "\xdb\xab\xa5\x3e", 4);
  struct csum_inet csum_1;
  csum_inet_init(&csum_1);
  csum_inet_update(&csum_1, (const uint8_t*)0x20634016, 4);
  csum_inet_update(&csum_1, (const uint8_t*)0x2063401a, 4);
  uint16_t csum_1_chunk_2 = 0x1100;
  csum_inet_update(&csum_1, (const uint8_t*)&csum_1_chunk_2, 2);
  uint16_t csum_1_chunk_3 = 0xc00;
  csum_inet_update(&csum_1, (const uint8_t*)&csum_1_chunk_3, 2);
  csum_inet_update(&csum_1, (const uint8_t*)0x2063401e, 12);
  *(uint16_t*)0x20634024 = csum_inet_digest(&csum_1);
  struct csum_inet csum_2;
  csum_inet_init(&csum_2);
  csum_inet_update(&csum_2, (const uint8_t*)0x2063400a, 20);
  *(uint16_t*)0x20634014 = csum_inet_digest(&csum_2);
  syscall(__NR_write, r[2], 0x20634000, 0x2a);
}

int main()
{
  use_temporary_dir();
  int pid = do_sandbox_namespace(0, false);
  int status = 0;
  while (waitpid(pid, &status, __WALL) != pid) {
  }
  return 0;
}