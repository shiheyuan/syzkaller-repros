// possible deadlock in do_ip_setsockopt
// https://syzkaller.appspot.com/bug?id=6366fc0d4b330eb0f95a736b02f14612697366b8
// status:open
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

uint64_t r[2] = {0xffffffffffffffff, 0xffffffffffffffff};
void loop()
{
  long res = 0;
  res = syscall(__NR_socket, 2, 3, 2);
  if (res != -1)
    r[0] = res;
  *(uint32_t*)0x20000080 = 0;
  syscall(__NR_setsockopt, r[0], 0, 0xc8, 0x20000080, 4);
  *(uint32_t*)0x20000000 = 0;
  syscall(__NR_setsockopt, r[0], 0, 5, 0x20000000, 4);
  res = syscall(__NR_socket, 2, 0x4000000000000001, 0);
  if (res != -1)
    r[1] = res;
  *(uint32_t*)0x20000000 = 0;
  *(uint16_t*)0x20000008 = 2;
  *(uint16_t*)0x2000000a = htobe16(0);
  *(uint8_t*)0x2000000c = 0xac;
  *(uint8_t*)0x2000000d = 0x14;
  *(uint8_t*)0x2000000e = 0x14;
  *(uint8_t*)0x2000000f = 0xbb;
  *(uint8_t*)0x20000010 = 0;
  *(uint8_t*)0x20000011 = 0;
  *(uint8_t*)0x20000012 = 0;
  *(uint8_t*)0x20000013 = 0;
  *(uint8_t*)0x20000014 = 0;
  *(uint8_t*)0x20000015 = 0;
  *(uint8_t*)0x20000016 = 0;
  *(uint8_t*)0x20000017 = 0;
  syscall(__NR_setsockopt, r[1], 0, 0x2a, 0x20000000, 0x88);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}