// KASAN: slab-out-of-bounds Read in bpf_skb_change_proto
// https://syzkaller.appspot.com/bug?id=699ab57701c240ba847bd831e714aeda5b56047b
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

#ifndef __NR_bpf
#define __NR_bpf 321
#endif

uint64_t r[1] = {0xffffffffffffffff};

int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  intptr_t res = 0;
  *(uint32_t*)0x20000200 = 4;
  *(uint32_t*)0x20000204 = 0xe;
  *(uint64_t*)0x20000208 = 0x20000380;
  memcpy((void*)0x20000380,
         "\xb7\x02\x00\x00\x04\x00\x00\x00\xbf\xa3\x00\x00\x00\x00\x00\x00\x07"
         "\x01\x00\x00\x00\xfe\xff\xff\x7a\x0a\xf0\xff\xf8\xff\xff\xff\x79\xa4"
         "\xf0\xff\x00\x00\x00\x00\xb7\x06\x00\x00\xff\xff\xff\xff\x2d\x64\x05"
         "\x00\x00\x00\x00\x00\x65\x04\x04\x00\x01\x00\x00\x00\x04\x04\x00\x00"
         "\x01\x00\x7d\x60\xb7\x03\x00\x00\x00\x00\x00\x00\x6a\x0a\x00\xfe\x00"
         "\x00\x00\x00\x85\x00\x00\x00\x1f\x00\x00\x00\xb7\x00\x00\x00\x00\x00"
         "\x00\x00\x95\x00\x00\x00\x00\x00\x00\x00\x61\xd4\x6e\x17\x8f\x62\xbe"
         "\xd0\x7c\x6a\x68\x50\xd2\x78\x24\xca\x3f\x33\x63\xfa\xbd\xde\xe2\xe4"
         "\x0e\x2b\x42\x46\xbc\xcb\xa3\x98\x67\xc2\x6c\xfb\xe0\x1d\xb5\x26\x02"
         "\xf8\x83\x63\x6c\x67\xf0\xfe\xcf\x72\x4f\x41\x64\x56\xa8\x7c\x7d\x6e"
         "\x0f\xdd\xf5\xd5\xc6\xea\x52\x3f\x0a\x46\xd0\xb1\x9b\xea\x60\xc5\xb7"
         "\x77\xfc\x97\x24\x02\x54\x84\xa4\x9c\x89\xdc\xb2\xb8\x1c\x6f\x37\x4e"
         "\xf5\x7b\xc5\xd0\x29\xf9\xf0\x60\x30\x44\x28\xf3\x58\x78\xa7\xec\xa7"
         "\xf9\x5c\x51\x48\xf8\x21\x02\xd0\x5e\xec\xc2\xb6\x94\x67\x2e\x67\x22"
         "\xc0\x8c\x50\x7e\xbc\x2b\x46\xa7\x79\xce\xe2\x40\xa0\x98\x15\x9a\x3f"
         "\x33\xcf\xa3\xa1\x98\xb8\x6e\x47\xa5\x7d\x07\x47\x40\xc1\xf8\x22\xb9"
         "\xcf\x7b\x8f\xa7\x7b\x8d\x47\x95\xca\x97\xec\x67\xb5\x12\x44\x7b\x66"
         "\xfe\x82\xef\x97\x90\x17\x9b\x9d\xde\x8c\xfd\x98\x35\xdc\x54\x65\xc0"
         "\x4f\x6c\x73\x5c\x92\x3e\xd9\x99\xce\x50\x88\x31\x67\x0c\xd2\x1a\x4e"
         "\xd8\x01\x1e\x7b\x11\xf4\x0c\x13\xd8\x04\xc0\xfe\x46\x41\xce\xb3\x9d"
         "\x16\xc2\x49\x84\xc2\x76\xfa\x1b\xdb\x0f\xa3\x50\xdf\xdb\x63\xcf\x06"
         "\x60\x8c\x7f\x09\xc8\x1f\x7c\x74\x63\xc3\x4c\x92",
         369);
  *(uint64_t*)0x20000210 = 0x20000340;
  memcpy((void*)0x20000340, "syzkaller\000", 10);
  *(uint32_t*)0x20000218 = 0;
  *(uint32_t*)0x2000021c = 0;
  *(uint64_t*)0x20000220 = 0;
  *(uint32_t*)0x20000228 = 0;
  *(uint32_t*)0x2000022c = 0;
  *(uint8_t*)0x20000230 = 0;
  *(uint8_t*)0x20000231 = 0;
  *(uint8_t*)0x20000232 = 0;
  *(uint8_t*)0x20000233 = 0;
  *(uint8_t*)0x20000234 = 0;
  *(uint8_t*)0x20000235 = 0;
  *(uint8_t*)0x20000236 = 0;
  *(uint8_t*)0x20000237 = 0;
  *(uint8_t*)0x20000238 = 0;
  *(uint8_t*)0x20000239 = 0;
  *(uint8_t*)0x2000023a = 0;
  *(uint8_t*)0x2000023b = 0;
  *(uint8_t*)0x2000023c = 0;
  *(uint8_t*)0x2000023d = 0;
  *(uint8_t*)0x2000023e = 0;
  *(uint8_t*)0x2000023f = 0;
  *(uint32_t*)0x20000240 = 0;
  *(uint32_t*)0x20000244 = 0;
  *(uint32_t*)0x20000248 = -1;
  *(uint32_t*)0x2000024c = 8;
  *(uint64_t*)0x20000250 = 0x20000000;
  *(uint32_t*)0x20000000 = 0;
  *(uint32_t*)0x20000004 = 0;
  *(uint32_t*)0x20000258 = 0;
  *(uint32_t*)0x2000025c = 0x10;
  *(uint64_t*)0x20000260 = 0x20000000;
  *(uint32_t*)0x20000000 = 0;
  *(uint32_t*)0x20000004 = 0;
  *(uint32_t*)0x20000008 = 0;
  *(uint32_t*)0x2000000c = 0;
  *(uint32_t*)0x20000268 = 0;
  res = syscall(__NR_bpf, 5, 0x20000200, 0x48);
  if (res != -1)
    r[0] = res;
  *(uint32_t*)0x20000280 = r[0];
  *(uint32_t*)0x20000284 = 0;
  *(uint32_t*)0x20000288 = 0xe;
  *(uint32_t*)0x2000028c = 0;
  *(uint64_t*)0x20000290 = 0x20000100;
  memcpy((void*)0x20000100,
         "\x3d\x7c\xe2\xe0\x4b\x91\xab\x10\x14\x3d\x0c\x0b\x86\xdd", 14);
  *(uint64_t*)0x20000298 = 0;
  *(uint32_t*)0x200002a0 = 0x8000;
  *(uint32_t*)0x200002a4 = 0;
  syscall(__NR_bpf, 0xa, 0x20000280, 0x28);
  return 0;
}
