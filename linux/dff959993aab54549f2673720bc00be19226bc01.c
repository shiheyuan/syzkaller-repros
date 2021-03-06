// general protection fault in bpf_skb_ancestor_cgroup_id
// https://syzkaller.appspot.com/bug?id=dff959993aab54549f2673720bc00be19226bc01
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
  syscall(__NR_mmap, 0x20000000ul, 0x1000000ul, 3ul, 0x32ul, -1, 0);
  intptr_t res = 0;
  *(uint32_t*)0x20000200 = 4;
  *(uint32_t*)0x20000204 = 0xe;
  *(uint64_t*)0x20000208 = 0x20000380;
  memcpy(
      (void*)0x20000380,
      "\xb7\x02\x00\x00\xff\x0f\x00\x00\xbf\xa3\x00\x00\x00\x00\x00\x00\x07\x03"
      "\x00\x00\x00\xfe\xff\xff\x7a\x0a\xf0\xff\xf8\xff\xff\xff\x79\xa4\xf0\xff"
      "\x00\x00\x00\x00\xb7\x06\x00\x00\xff\xff\xff\xff\x6d\x64\x05\x00\x00\x00"
      "\x00\x00\x65\x04\x04\x00\x01\x00\x00\x00\x04\x04\x00\x00\x01\x00\x00\x00"
      "\xb7\x05\x00\x00\x00\x00\x00\x00\x6a\x0a\x80\xfe\x00\x00\x00\x00\x85\x00"
      "\x00\x00\x53\x00\x00\x00\xb7\x00\x00\x00\x00\x00\x00\x00\x95\x00\x00\x10"
      "\x00\x00\x00\x00\x4e\x62\x01\x36\x30\x34\xfd\xb1\x17\x16\x8b\xd0\x7b\xa0"
      "\x8a\xf3\x39\xd1\xa1\xee\x35\xfe\x2a\x3a\x25\x5c\x33\x28\x20\x44\xb3\x24"
      "\x95\x3c\x0a\x9f\xa9\xa8\x44\x50\x56\x99\x57\xc1\x00\x2e\xd7\xd4\xd8\xe1"
      "\x7f\x79\x1f\x47\x98\xc8\xeb\x48\x4d\xe0\x33\x52\xc6\x9b\x3e\xdf\xf5\xbe"
      "\x26\x76\x5b\xa5\xf8\xf2\x87\x90\x21\xc2\xea\x53\xea\x79\xac\xd7\xfb\x38"
      "\xfd\xf7\x9f\x2b\xe9\x08\x7a\x3e\x7b\x7c\x4a\xe7\xdd\x5e\x4d\xee\x88\x51"
      "\xd4\x0c\x61\x7b\x58\xc8\x10\x8d\xdf\x12\xdd\xdd\x4b\xfc\x6a\x4d\xd3\xa6"
      "\x6e\xb2\x2d\x31\x28\x58\xf1\xf5\xb6\xbe\xba\x51\x0b\x42\x29\xb0\xd4\xb5"
      "\x04\x51\x6c\x4c\x3e\x5d\x1a\xa0\x44\xd8\xd0\x07\x28\x14\x1c\xd6\x7b\xcd"
      "\x68\xf2\x53\x28\x8e\x65\x5c\x6b\x34\xe0\x2e\x90\x63\x7e\xf2\x91\x2b\xa7"
      "\xde\x26\xff\x63\x57\xef\x17\xf9\x5a\x25\x78\x0c\x3a\x05\x78\x44\x29\x26"
      "\xef\x4e\x91\x2f\x01\xa2\x01\xe6\x94\xe3\x80\x6e\x8c\x8f\xe8\xb6\x95\x24"
      "\xcd\x19\xf7\x52\x5d\x8d\x66\xbb\x76\x6f\x7f\x00\x00\x00\x00\x00\x00\x00"
      "\x80\x68\xd9\xf8\x18\x97\x13\x3a\xf9\x4a\x5a\x4c\xfc\x79\x4d\x8b\x9d\x7c"
      "\x33\x63\x21\x52\xc5\x8e\xaf\x30\x2f\x0b\x2e\x0c\x25\x2b\xb6\x67\x62\xdb"
      "\xfa\x41\x77\xa5\xee\x91\x7b\xca\x48\x85\xbb\xf5\x97\xa1\x4a\xb2\x45\x8e"
      "\x62\x72\xd8\x8e\x0c\x80\x88\xf4\x04\xf0\x11\x28\x9e\xbc\x56\x23\xea\x21"
      "\x18\x26\x32\x16\x1e\x07\x3a\xf1\xd6\x9a\x2e\x3b\xbe\x11\x5b\xe3\xb3\x25"
      "\xec\xd2\x01\xd2\xff\xb0\xa7\xfa\x4f\x5d\x11\x06\x0c\xdc\xf0\x71\xde\xfd"
      "\x0a\x8b\xe3\xb6\x9c\xe3\xe4\xf3\x61\xac\xa7\x58\x27\x42\x6d\xde\x87\xfd"
      "\xf4\x61\x72\x22\x40\x64\x19\x86\x0d\x46\x69\xdf\x2d\xc9\xcc\x24\x0e\x40"
      "\x6b\xf0\xc5\x32\x9b\xb4\x69\x73\x36\x11\x2b\x0b\x87\x54\xce\x35\x74\x04"
      "\x6b\xf6\x11\xa1\x08\xf8\xdf\x4d\x1a\x88\x59\x78\x50\xb7\x02\xb6\xfa\x8e"
      "\xdf\xff\x8f\xaf\x8b\x8e\xc0\x20\xba\xb3\x6c\xca\xc0\x53\x53\x73\xbb\x8f"
      "\xab\x90\x53\x9b\x1a\x65\xdd\xff\x84\x1e\xb6\x71\xf7\xfa\xf3\x7e\xbd\xfc"
      "\xce\xa0\xc0\x02\xad\x2b\x42\x04\x7c\x9e\xc4\x31\x93\xcc\xf6\x17\xdb\xf8"
      "\xa1\x2b\x4a\x18\x9e\xdb\xf9\xfb\x7c\x42\xb1\xf4\x35\xec\xd4\xd9\x68\x22"
      "\xe6\xb7\x0b\x62\x91\x2c\x92\x6d\xbe\x41\x7c\xcc\xc4\xf6\x96\xd5\x28\xfa"
      "\x8a\x3e\xa8\x47\xf1\x0e\x74\x70\xf0\x91\xbb\x50\x6f\x1d\x7f\xbd\xf8\x4b"
      "\x1b\x06\xaf\x9f\xb6\x54\xeb\x6c\x02\x8e\xb5\xb5\xa0\x73\xd0\xde\x55\x38"
      "\xab\x42\xe1\x70\xb3\xba\xae\x34\xc3\x59\x87\xb0\xdd\xa4\x97\xac\x3f"
      "\x00",
      666);
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
  *(uint32_t*)0x2000026c = 0;
  *(uint32_t*)0x20000270 = -1;
  res = syscall(__NR_bpf, 5ul, 0x20000200ul, 0x48ul);
  if (res != -1)
    r[0] = res;
  *(uint32_t*)0x20000280 = r[0];
  *(uint32_t*)0x20000284 = 0x4000;
  *(uint32_t*)0x20000288 = 0x113;
  *(uint32_t*)0x2000028c = 0;
  *(uint64_t*)0x20000290 = 0x20000700;
  memcpy((void*)0x20000700,
         "\xc4\x5c\x57\xce\x39\x5d\xe5\xb2\x89\x0f\x7d\x63\x7a\x22\x39\x20\xf1"
         "\x81\xc2\xe5\x7d\x71\x48\x3c\xfb\x2d\x07\x5a\x3f\xa6\x72\x58\xe0\x80"
         "\xa1\x94\x80\x5c\xdb\x0c\x26\xd3\xf7\xff\xb1\xe0\xd9\xcf\x4f\xa3\x6d"
         "\xcb\x21\x68\xb7\x2d\xe4\x8a\xc8\xf9\x3e\x68\x04\xf1\xc4\xd7\x08\x98"
         "\xd0\x81\x0e\x04\x4d\x7e\x17\x78\xea\xac\x5d\xfd\xcc\x9f\x12\x08\x90"
         "\x55\x22\x02\x5b\xcf\xdf\x1b\xf0\x96\x9b\x09\x4d\x5c\x02\x2c\x2b\x7f"
         "\xfe\xfd\xe7\x1e\x06\x27\xb9\xa2\x06\x9c\xc1\xe0\x17\x5c\x4b\x88\x60"
         "\xaa\xd4\xb0\xa1\x03\xc5\x89\xf6\x76\xb6\xc4\xe8\x5e\xb3\x95\x0c\x53"
         "\x3b\x6e\x62\xc3\x9c\xcf\x9a\xe9\xbf\xe5\x4e\xe5\x88\x73\x58\xd4\x4f"
         "\x46\x33\x7f\xbe\x09\x0d\x7c\x7e\x55\x84\x7e\xde\xe8\x13\x0f\xfd\x3d"
         "\x1e\x71\x9e\x01\xa6\x8b\x0e\x69\x1c\x0d\x35\xb0\xb5\x6e\x0b\x51\x40"
         "\x36\x34\x2f\xd5\x6f\x08\xac\x00\x83\xf3\xc2\xfe\x41\xa1\x29\x5a\x3d"
         "\x23\xcf\x3d\x16\x0d\x4f\xd9\x0f\x66\xbe\xba\x68\x86\x04\x56\xed\x41"
         "\x27\x2e\x1e\x68\xd1\x6c\x25\x64\xc8\x5f\x55\x56\xe1\x87\x84\x11\x3c"
         "\x49\x3d\x13\x25\x3e\x14\xd6\xeb\x89\x17\x07\xfb\xa3\xc3\x0d\x07\xd5"
         "\xee\x86\x19\xe4\x42\x6c\xaf\xec\x4c\xf6\xa3\x72\x3c\x45\x5d\x09\xb5"
         "\x86\xb2\x48",
         275);
  *(uint64_t*)0x20000298 = 0;
  *(uint32_t*)0x200002a0 = 0xf0;
  *(uint32_t*)0x200002a4 = 0;
  *(uint32_t*)0x200002a8 = 0x4a;
  *(uint32_t*)0x200002ac = 0x2c4;
  *(uint64_t*)0x200002b0 = 0x20000000;
  *(uint64_t*)0x200002b8 = 0x20000000;
  syscall(__NR_bpf, 0xaul, 0x20000280ul, 0x40ul);
  return 0;
}
