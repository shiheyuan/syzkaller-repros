// KMSAN: kernel-infoleak in video_usercopy (2)
// https://syzkaller.appspot.com/bug?id=eb27a56e5ac9b45d1de97cfbe31f98628fa17d8d
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

uint64_t r[1] = {0xffffffffffffffff};

int main(void)
{
  syscall(__NR_mmap, 0x20000000ul, 0x1000000ul, 3ul, 0x32ul, -1, 0);
  intptr_t res = 0;
  memcpy((void*)0x20000040, "/dev/video36\000", 13);
  res = syscall(__NR_openat, 0xffffffffffffff9cul, 0x20000040ul, 2ul, 0ul);
  if (res != -1)
    r[0] = res;
  *(uint32_t*)0x20000100 = 8;
  *(uint32_t*)0x20000108 = 0x32314d4e;
  *(uint32_t*)0x2000010c = 0x400;
  *(uint8_t*)0x20000110 = 0;
  *(uint8_t*)0x20000111 = 0;
  *(uint8_t*)0x20000112 = 0;
  *(uint8_t*)0x20000113 = 0;
  *(uint8_t*)0x20000114 = 0;
  *(uint8_t*)0x20000115 = 0;
  *(uint8_t*)0x20000116 = 0;
  *(uint8_t*)0x20000117 = 0;
  *(uint8_t*)0x20000118 = 0;
  *(uint8_t*)0x20000119 = 0;
  *(uint8_t*)0x2000011a = 0;
  *(uint8_t*)0x2000011b = 0;
  *(uint8_t*)0x2000011c = 0;
  *(uint8_t*)0x2000011d = 0;
  *(uint8_t*)0x2000011e = 0;
  *(uint8_t*)0x2000011f = 0;
  *(uint8_t*)0x20000120 = 0;
  *(uint8_t*)0x20000121 = 0;
  *(uint8_t*)0x20000122 = 0;
  *(uint8_t*)0x20000123 = 0;
  *(uint8_t*)0x20000124 = 0;
  *(uint8_t*)0x20000125 = 0;
  *(uint8_t*)0x20000126 = 0;
  *(uint8_t*)0x20000127 = 0;
  syscall(__NR_ioctl, -1, 0xc0d05604ul, 0x20000100ul);
  *(uint32_t*)0x200000c0 = 0;
  *(uint32_t*)0x200000c4 = 0xa;
  *(uint32_t*)0x200000c8 = 0;
  *(uint32_t*)0x200000cc = 0;
  *(uint32_t*)0x200000d0 = 0;
  syscall(__NR_ioctl, r[0], 0xc050560ful, 0x200000c0ul);
  return 0;
}
