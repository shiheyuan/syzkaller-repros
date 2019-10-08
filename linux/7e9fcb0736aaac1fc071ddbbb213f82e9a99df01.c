// possible deadlock in lock_trace
// https://syzkaller.appspot.com/bug?id=7e9fcb0736aaac1fc071ddbbb213f82e9a99df01
// status:dup
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <errno.h>
#include <fcntl.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_tun.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <net/if_arp.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/wait.h>
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

static void vsnprintf_check(char* str, size_t size, const char* format,
                            va_list args)
{
  int rv;

  rv = vsnprintf(str, size, format, args);
  if (rv < 0)
    fail("tun: snprintf failed");
  if ((size_t)rv >= size)
    fail("tun: string '%s...' doesn't fit into buffer", str);
}

static void snprintf_check(char* str, size_t size, const char* format,
                           ...)
{
  va_list args;

  va_start(args, format);
  vsnprintf_check(str, size, format, args);
  va_end(args);
}

#define COMMAND_MAX_LEN 128
#define PATH_PREFIX                                                    \
  "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin "
#define PATH_PREFIX_LEN (sizeof(PATH_PREFIX) - 1)

static void execute_command(const char* format, ...)
{
  va_list args;
  char command[PATH_PREFIX_LEN + COMMAND_MAX_LEN];
  int rv;

  va_start(args, format);
  memcpy(command, PATH_PREFIX, PATH_PREFIX_LEN);
  vsnprintf_check(command + PATH_PREFIX_LEN, COMMAND_MAX_LEN, format,
                  args);
  rv = system(command);
  if (rv != 0)
    fail("tun: command \"%s\" failed with code %d", &command[0], rv);

  va_end(args);
}

static int tunfd = -1;
static int tun_frags_enabled;

#define SYZ_TUN_MAX_PACKET_SIZE 1000

#define MAX_PIDS 32
#define ADDR_MAX_LEN 32

#define LOCAL_MAC "aa:aa:aa:aa:aa:%02hx"
#define REMOTE_MAC "bb:bb:bb:bb:bb:%02hx"

#define LOCAL_IPV4 "172.20.%d.170"
#define REMOTE_IPV4 "172.20.%d.187"

#define LOCAL_IPV6 "fe80::%02hxaa"
#define REMOTE_IPV6 "fe80::%02hxbb"

#define IFF_NAPI 0x0010
#define IFF_NAPI_FRAGS 0x0020

static void initialize_tun(uint64_t pid)
{
  if (pid >= MAX_PIDS)
    fail("tun: no more than %d executors", MAX_PIDS);
  int id = pid;

  tunfd = open("/dev/net/tun", O_RDWR | O_NONBLOCK);
  if (tunfd == -1) {
    printf(
        "tun: can't open /dev/net/tun: please enable CONFIG_TUN=y\n");
    printf("otherwise fuzzing or reproducing might not work as "
           "intended\n");
    return;
  }

  char iface[IFNAMSIZ];
  snprintf_check(iface, sizeof(iface), "syz%d", id);

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, iface, IFNAMSIZ);
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI | IFF_NAPI | IFF_NAPI_FRAGS;
  if (ioctl(tunfd, TUNSETIFF, (void*)&ifr) < 0) {
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (ioctl(tunfd, TUNSETIFF, (void*)&ifr) < 0)
      fail("tun: ioctl(TUNSETIFF) failed");
  }
  if (ioctl(tunfd, TUNGETIFF, (void*)&ifr) < 0)
    fail("tun: ioctl(TUNGETIFF) failed");
  tun_frags_enabled = (ifr.ifr_flags & IFF_NAPI_FRAGS) != 0;

  char local_mac[ADDR_MAX_LEN];
  snprintf_check(local_mac, sizeof(local_mac), LOCAL_MAC, id);
  char remote_mac[ADDR_MAX_LEN];
  snprintf_check(remote_mac, sizeof(remote_mac), REMOTE_MAC, id);

  char local_ipv4[ADDR_MAX_LEN];
  snprintf_check(local_ipv4, sizeof(local_ipv4), LOCAL_IPV4, id);
  char remote_ipv4[ADDR_MAX_LEN];
  snprintf_check(remote_ipv4, sizeof(remote_ipv4), REMOTE_IPV4, id);

  char local_ipv6[ADDR_MAX_LEN];
  snprintf_check(local_ipv6, sizeof(local_ipv6), LOCAL_IPV6, id);
  char remote_ipv6[ADDR_MAX_LEN];
  snprintf_check(remote_ipv6, sizeof(remote_ipv6), REMOTE_IPV6, id);

  execute_command("sysctl -w net.ipv6.conf.%s.accept_dad=0", iface);

  execute_command("sysctl -w net.ipv6.conf.%s.router_solicitations=0",
                  iface);

  execute_command("ip link set dev %s address %s", iface, local_mac);
  execute_command("ip addr add %s/24 dev %s", local_ipv4, iface);
  execute_command("ip -6 addr add %s/120 dev %s", local_ipv6, iface);
  execute_command("ip neigh add %s lladdr %s dev %s nud permanent",
                  remote_ipv4, remote_mac, iface);
  execute_command("ip -6 neigh add %s lladdr %s dev %s nud permanent",
                  remote_ipv6, remote_mac, iface);
  execute_command("ip link set dev %s up", iface);
}

static void setup_tun(uint64_t pid, bool enable_tun)
{
  if (enable_tun)
    initialize_tun(pid);
}

static uintptr_t syz_open_procfs(uintptr_t a0, uintptr_t a1)
{

  char buf[128];
  memset(buf, 0, sizeof(buf));
  if (a0 == 0) {
    snprintf(buf, sizeof(buf), "/proc/self/%s", (char*)a1);
  } else if (a0 == (uintptr_t)-1) {
    snprintf(buf, sizeof(buf), "/proc/thread-self/%s", (char*)a1);
  } else {
    snprintf(buf, sizeof(buf), "/proc/self/task/%d/%s", (int)a0,
             (char*)a1);
  }
  int fd = open(buf, O_RDWR);
  if (fd == -1)
    fd = open(buf, O_RDONLY);
  return fd;
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

static int do_sandbox_none(int executor_pid, bool enable_tun)
{
  unshare(CLONE_NEWPID);
  int pid = fork();
  if (pid < 0)
    fail("sandbox fork failed");
  if (pid)
    return pid;

  sandbox_common();
  setup_tun(executor_pid, enable_tun);

  loop();
  doexit(1);
}

static void test();

void loop()
{
  while (1) {
    test();
  }
}

long r[28];
void* thr(void* arg)
{
  switch ((long)arg) {
  case 0:
    r[0] = syscall(__NR_mmap, 0x20000000ul, 0xfff000ul, 0x3ul, 0x32ul,
                   0xfffffffffffffffful, 0x0ul);
    break;
  case 1:
    memcpy((void*)0x201c2000, "\x2e\x2f\x66\x69\x6c\x65\x30\x00", 8);
    r[2] = syscall(__NR_mknod, 0x201c2000ul, 0x1040ul, 0x0ul);
    break;
  case 2:
    memcpy((void*)0x20f8aff8, "\x2e\x2f\x66\x69\x6c\x65\x30\x00", 8);
    *(uint64_t*)0x20030000 = (uint64_t)0x2085bfa6;
    *(uint64_t*)0x20030008 = (uint64_t)0x2015effa;
    *(uint64_t*)0x20030010 = (uint64_t)0x20219ff4;
    *(uint64_t*)0x20030018 = (uint64_t)0x204ea000;
    *(uint64_t*)0x20030020 = (uint64_t)0x20e0aff0;
    *(uint64_t*)0x20030028 = (uint64_t)0x2019f000;
    *(uint64_t*)0x20030030 = (uint64_t)0x20d8c000;
    *(uint64_t*)0x20030038 = (uint64_t)0x2072c000;
    *(uint64_t*)0x20030040 = (uint64_t)0x20ffe000;
    memcpy((void*)0x2085bfa6,
           "\x75\x73\x65\x72\x5d\x2a\x6d\x69\x6d\x65\x5f\x74\x79\x70"
           "\x65\x6d\x64\x35\x73\x75\x6d\x7b\x5d\x74\x72\x75\x73\x74"
           "\x65\x64\x73\x65\x6c\x69\x6e\x75\x78\x16\x26\x75\x73\x65"
           "\x72\x73\x65\x63\x75\x72\x69\x74\x79\x2c\x24\x76\x62\x6f"
           "\x78\x6e\x65\x74\x31\x26\x2b\x5b\x47\x50\x4c\x76\x62\x6f"
           "\x78\x6e\x65\x74\x31\x6d\x64\x35\x73\x75\x6d\xcf\x65\x74"
           "\x68\x30\x47\x50\x4c\x00",
           90);
    memcpy((void*)0x2015effa, "\x6c\x6f\x67\x6f\x6e\x00", 6);
    memcpy((void*)0x20219ff4,
           "\x65\x6d\x31\x73\x65\x63\x75\x72\x69\x74\x79\x00", 12);
    memcpy((void*)0x204ea000,
           "\x63\x70\x75\x73\x65\x74\x7d\x25\x25\x00", 10);
    memcpy((void*)0x20e0aff0, "\x2f\x64\x65\x76\x2f\x73\x65\x71\x75\x65"
                              "\x6e\x63\x65\x72\x32\x00",
           16);
    memcpy((void*)0x2019f000, "\x21\x7b\x00", 3);
    memcpy((void*)0x20d8c000, "\x65\x6d\x31\x5b\x2c\x00", 6);
    memcpy((void*)0x2072c000,
           "\x40\x73\x65\x63\x75\x72\x69\x74\x79\x2d\x00", 11);
    memcpy((void*)0x20ffe000, "\x00", 1);
    r[22] =
        syscall(__NR_execve, 0x20f8aff8ul, 0x2089d000ul, 0x20030000ul);
    break;
  case 3:
    r[23] = syscall(__NR_pipe2, 0x20c42000ul, 0x0ul);
    if (r[23] != -1)
      r[24] = *(uint32_t*)0x20c42004;
    break;
  case 4:
    memcpy((void*)0x20e03ff8, "\x73\x79\x73\x63\x61\x6c\x6c\x00", 8);
    r[26] = syz_open_procfs(0x0ul, 0x20e03ff8ul);
    break;
  case 5:
    r[27] = syscall(__NR_splice, r[26], 0x0ul, r[24], 0x0ul, 0xa3eaul,
                    0x0ul);
    break;
  }
  return 0;
}

void test()
{
  long i;
  pthread_t th[12];

  memset(r, -1, sizeof(r));
  for (i = 0; i < 6; i++) {
    pthread_create(&th[i], 0, thr, (void*)i);
    usleep(rand() % 10000);
  }
  usleep(rand() % 100000);
}

int main()
{
  int pid = do_sandbox_none(0, true);
  int status = 0;
  while (waitpid(pid, &status, __WALL) != pid) {
  }
  return 0;
}