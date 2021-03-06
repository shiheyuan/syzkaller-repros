// KASAN: stack-out-of-bounds Read in memcmp
// https://syzkaller.appspot.com/bug?id=b6357f82f451a979ea81d03c1e7a151519bd4c5b
// status:fixed
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_tun.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <net/if_arp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

const int kFailStatus = 67;
const int kRetryStatus = 69;

__attribute__((noreturn)) static void doexit(int status)
{
  volatile unsigned i;
  syscall(__NR_exit_group, status);
  for (i = 0;; i++) {
  }
}

__attribute__((noreturn)) static void fail(const char* msg, ...)
{
  int e = errno;
  fflush(stdout);
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

static void execute_command(const char* format, ...)
{
  va_list args;
  char command[COMMAND_MAX_LEN];
  int rv;

  va_start(args, format);

  vsnprintf_check(command, sizeof(command), format, args);
  rv = system(command);
  if (rv != 0)
    fail("tun: command \"%s\" failed with code %d", &command[0], rv);

  va_end(args);
}

static int tunfd = -1;

#define SYZ_TUN_MAX_PACKET_SIZE 1000

#define MAX_PIDS 32
#define ADDR_MAX_LEN 32

#define LOCAL_MAC "aa:aa:aa:aa:aa:%02hx"
#define REMOTE_MAC "bb:bb:bb:bb:bb:%02hx"

#define LOCAL_IPV4 "172.20.%d.170"
#define REMOTE_IPV4 "172.20.%d.187"

#define LOCAL_IPV6 "fe80::%02hxaa"
#define REMOTE_IPV6 "fe80::%02hxbb"

static void initialize_tun(uint64_t pid)
{
  if (pid >= MAX_PIDS)
    fail("tun: no more than %d executors", MAX_PIDS);
  int id = pid;

  tunfd = open("/dev/net/tun", O_RDWR | O_NONBLOCK);
  if (tunfd == -1)
    fail("tun: can't open /dev/net/tun");

  char iface[IFNAMSIZ];
  snprintf_check(iface, sizeof(iface), "syz%d", id);

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, iface, IFNAMSIZ);
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
  if (ioctl(tunfd, TUNSETIFF, (void*)&ifr) < 0)
    fail("tun: ioctl(TUNSETIFF) failed");

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

static int read_tun(char* data, int size)
{
  int rv = read(tunfd, data, size);
  if (rv < 0) {
    if (errno == EAGAIN)
      return -1;
    fail("tun: read failed with %d, errno: %d", rv, errno);
  }
  return rv;
}

struct ipv6hdr {
  __u8 priority : 4, version : 4;
  __u8 flow_lbl[3];

  __be16 payload_len;
  __u8 nexthdr;
  __u8 hop_limit;

  struct in6_addr saddr;
  struct in6_addr daddr;
};

struct tcp_resources {
  int32_t seq;
  int32_t ack;
};

static uintptr_t syz_extract_tcp_res(uintptr_t a0, uintptr_t a1,
                                     uintptr_t a2)
{

  if (tunfd < 0)
    return (uintptr_t)-1;

  char data[SYZ_TUN_MAX_PACKET_SIZE];
  int rv = read_tun(&data[0], sizeof(data));
  if (rv == -1)
    return (uintptr_t)-1;
  size_t length = rv;

  struct tcphdr* tcphdr;

  if (length < sizeof(struct ethhdr))
    return (uintptr_t)-1;
  struct ethhdr* ethhdr = (struct ethhdr*)&data[0];

  if (ethhdr->h_proto == htons(ETH_P_IP)) {
    if (length < sizeof(struct ethhdr) + sizeof(struct iphdr))
      return (uintptr_t)-1;
    struct iphdr* iphdr = (struct iphdr*)&data[sizeof(struct ethhdr)];
    if (iphdr->protocol != IPPROTO_TCP)
      return (uintptr_t)-1;
    if (length <
        sizeof(struct ethhdr) + iphdr->ihl * 4 + sizeof(struct tcphdr))
      return (uintptr_t)-1;
    tcphdr =
        (struct tcphdr*)&data[sizeof(struct ethhdr) + iphdr->ihl * 4];
  } else {
    if (length < sizeof(struct ethhdr) + sizeof(struct ipv6hdr))
      return (uintptr_t)-1;
    struct ipv6hdr* ipv6hdr =
        (struct ipv6hdr*)&data[sizeof(struct ethhdr)];
    if (ipv6hdr->nexthdr != IPPROTO_TCP)
      return (uintptr_t)-1;
    if (length < sizeof(struct ethhdr) + sizeof(struct ipv6hdr) +
                     sizeof(struct tcphdr))
      return (uintptr_t)-1;
    tcphdr = (struct tcphdr*)&data[sizeof(struct ethhdr) +
                                   sizeof(struct ipv6hdr)];
  }

  struct tcp_resources* res = (struct tcp_resources*)a0;
  res->seq = htonl((ntohl(tcphdr->seq) + (uint32_t)a1));
  res->ack = htonl((ntohl(tcphdr->ack_seq) + (uint32_t)a2));

  return 0;
}

long r[77];
void loop()
{
  memset(r, -1, sizeof(r));
  r[0] = syscall(__NR_mmap, 0x20000000ul, 0xfff000ul, 0x3ul, 0x32ul,
                 r[0], 0x0ul);
  r[1] = syscall(__NR_socket, 0xaul, 0x2ul, 0x0ul);
  *(uint8_t*)0x20000f18 = (uint8_t)0x0;
  *(uint8_t*)0x20000f19 = (uint8_t)0x0;
  *(uint8_t*)0x20000f1a = (uint8_t)0x0;
  *(uint8_t*)0x20000f1b = (uint8_t)0x0;
  *(uint8_t*)0x20000f1c = (uint8_t)0x0;
  *(uint8_t*)0x20000f1d = (uint8_t)0x0;
  *(uint8_t*)0x20000f1e = (uint8_t)0x0;
  *(uint8_t*)0x20000f1f = (uint8_t)0x0;
  *(uint8_t*)0x20000f20 = (uint8_t)0x0;
  *(uint8_t*)0x20000f21 = (uint8_t)0x0;
  *(uint8_t*)0x20000f22 = (uint8_t)0x0;
  *(uint8_t*)0x20000f23 = (uint8_t)0x0;
  *(uint8_t*)0x20000f24 = (uint8_t)0x0;
  *(uint8_t*)0x20000f25 = (uint8_t)0x0;
  *(uint8_t*)0x20000f26 = (uint8_t)0x0;
  *(uint8_t*)0x20000f27 = (uint8_t)0x0;
  *(uint64_t*)0x20000f28 = (uint64_t)0x0;
  *(uint64_t*)0x20000f30 = (uint64_t)0x100000000000000;
  *(uint16_t*)0x20000f38 = (uint16_t)0x204e;
  *(uint16_t*)0x20000f3a = (uint16_t)0x0;
  *(uint16_t*)0x20000f3c = (uint16_t)0x204e;
  *(uint16_t*)0x20000f3e = (uint16_t)0x0;
  *(uint16_t*)0x20000f40 = (uint16_t)0xa;
  *(uint8_t*)0x20000f42 = (uint8_t)0x0;
  *(uint8_t*)0x20000f43 = (uint8_t)0xa0;
  *(uint8_t*)0x20000f44 = (uint8_t)0x0;
  *(uint32_t*)0x20000f48 = r[0];
  *(uint32_t*)0x20000f4c = r[0];
  *(uint64_t*)0x20000f50 = (uint64_t)0x0;
  *(uint64_t*)0x20000f58 = (uint64_t)0x0;
  *(uint64_t*)0x20000f60 = (uint64_t)0x0;
  *(uint64_t*)0x20000f68 = (uint64_t)0x0;
  *(uint64_t*)0x20000f70 = (uint64_t)0x0;
  *(uint64_t*)0x20000f78 = (uint64_t)0x0;
  *(uint64_t*)0x20000f80 = (uint64_t)0x0;
  *(uint64_t*)0x20000f88 = (uint64_t)0x0;
  *(uint64_t*)0x20000f90 = (uint64_t)0x0;
  *(uint64_t*)0x20000f98 = (uint64_t)0x0;
  *(uint64_t*)0x20000fa0 = (uint64_t)0x0;
  *(uint64_t*)0x20000fa8 = (uint64_t)0x0;
  *(uint32_t*)0x20000fb0 = (uint32_t)0x0;
  *(uint32_t*)0x20000fb4 = (uint32_t)0x0;
  *(uint8_t*)0x20000fb8 = (uint8_t)0x1;
  *(uint8_t*)0x20000fb9 = (uint8_t)0x0;
  *(uint8_t*)0x20000fba = (uint8_t)0x0;
  *(uint8_t*)0x20000fbb = (uint8_t)0x0;
  *(uint32_t*)0x20000fc0 = (uint32_t)0x20000e0;
  *(uint32_t*)0x20000fd0 = (uint32_t)0x4;
  *(uint8_t*)0x20000fd4 = (uint8_t)0x7;
  *(uint16_t*)0x20000fd8 = (uint16_t)0xa;
  *(uint32_t*)0x20000fdc = (uint32_t)0x100007f;
  *(uint32_t*)0x20000fec = (uint32_t)0x21a9336f;
  *(uint8_t*)0x20000ff0 = (uint8_t)0x6;
  *(uint8_t*)0x20000ff1 = (uint8_t)0x0;
  *(uint8_t*)0x20000ff2 = (uint8_t)0xff;
  *(uint32_t*)0x20000ff4 = (uint32_t)0x7fff;
  *(uint32_t*)0x20000ff8 = (uint32_t)0x1;
  *(uint32_t*)0x20000ffc = (uint32_t)0x401;
  r[60] = syscall(__NR_setsockopt, r[1], 0x29ul, 0x23ul, 0x20000f18ul,
                  0xe8ul);
  *(uint16_t*)0x20d0d000 = (uint16_t)0x2;
  *(uint16_t*)0x20d0d002 = (uint16_t)0x234e;
  *(uint8_t*)0x20d0d004 = (uint8_t)0xac;
  *(uint8_t*)0x20d0d005 = (uint8_t)0x14;
  *(uint8_t*)0x20d0d006 = (uint8_t)0x0;
  *(uint8_t*)0x20d0d007 = (uint8_t)0xbb;
  *(uint8_t*)0x20d0d008 = (uint8_t)0x0;
  *(uint8_t*)0x20d0d009 = (uint8_t)0x0;
  *(uint8_t*)0x20d0d00a = (uint8_t)0x0;
  *(uint8_t*)0x20d0d00b = (uint8_t)0x0;
  *(uint8_t*)0x20d0d00c = (uint8_t)0x0;
  *(uint8_t*)0x20d0d00d = (uint8_t)0x0;
  *(uint8_t*)0x20d0d00e = (uint8_t)0x0;
  *(uint8_t*)0x20d0d00f = (uint8_t)0x0;
  r[75] = syscall(__NR_sendto, r[1], 0x20dec000ul, 0x0ul, 0x0ul,
                  0x20d0d000ul, 0x10ul);
  r[76] = syz_extract_tcp_res(0x20000ff8ul, 0x5bul, 0x5ul);
}

int main()
{
  setup_tun(0, true);
  loop();
  return 0;
}
