// general protection fault in dccp_timeout_nlattr_to_obj
// https://syzkaller.appspot.com/bug?id=b44aa2c301f6eaf8d39a53b59cb3b441a468d9cf
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
  res = syscall(__NR_socket, 0x10ul, 3ul, 0xcul);
  if (res != -1)
    r[0] = res;
  *(uint64_t*)0x20000200 = 0x20000000;
  *(uint16_t*)0x20000000 = 0x10;
  *(uint16_t*)0x20000002 = 0;
  *(uint32_t*)0x20000004 = 0;
  *(uint32_t*)0x20000008 = 0;
  *(uint32_t*)0x20000208 = 0xc;
  *(uint64_t*)0x20000210 = 0x20000040;
  *(uint64_t*)0x20000040 = 0x20000440;
  memcpy((void*)0x20000440,
         "\xfc\x00\x00\x00\x03\x08\x01\x00\x2d\xbd\x70\x00\xff\xdb\xdf\xd0\x03"
         "\x00\x00\x02\x0c\x00\x01\x00\x73\x79\x7a\x31\x00\x00\x00\x00\x08\x00"
         "\x03\x00\xe0\xff\xff\x06\x08\x00\x06\x00\x00\x00\x00\x00\x0c\x00\x04"
         "\x00\x08\x00\x05\x00\x00\x00\x80\x01\x08\x00\x03\x00\x21\x00\x03\xff"
         "\xa0\x00\x02\x00\x08\x00\x03\x00\x00\x01\x00\x00\x08\x00\x03\x00\x00"
         "\x00\x00\x00\x08\x00\x03\x00\x00\x00\x00\x00\x21\xab\x3d\x7d\x43\xca"
         "\xfe\xc0\x9f\x91\x99\x9e\x58\x14\x21\x21\x72\x4d\xc7\xca\x1f\xac\x72"
         "\x8e\x14\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x01\x08\x00\x03\x00\x00\x00\x00\x00\x14\x00\x01\x00\x08"
         "\x00\x01\x00\xac\x1e\x00\x01\x08\x00\x02\x00\x7f\x00\x00\x01\x14\x00"
         "\x01\x00\x08\x00\x01\x00\xac\x1e\x01\x01\x08\x00\x02\x00\xff\xff\xff"
         "\xff\x08\x00\x03\x00\x00\x04\x00\x00\x0c\x00\x02\x00\x08\x00\x01\x00"
         "\x3a\x00\x00\x00\x14\x00\x01\x00\x08\x00\x01\x00\xac\x1e\x01\x01\x08"
         "\x00\x02\x00\xe0\x00\x00\x02\x08\x1c\x06\x00\x00\x00\x00\x00\x08\x00"
         "\x86\x00\x00\x00\x00\x01\x08\x00\x05\x00\x00\x00\x00\x15\xeb\xf4\xac"
         "\xa3\x58\xd0\xe9\xf8\x14\x89\x08\xa2\x80\x50\x2a\xb7\x18\x0d\x54\x5a"
         "\x75\xd3\x82\xcd\x56\x52\x37\x17\xe6\x25\x76\x3b\xff\xe7\x69\x44\x66"
         "\xce\xa0\x75\xf9\x0d\xa3\xb2\xd1\x7a\x3a\x52\x0d\xb0\xdd\xb9\xa3\x94"
         "\x28\x24\x4b\x4c\x62\x6e\x12\x24\x7f\xba\xfa\xd4\x08\xaa\xdc\x5b\xf3"
         "\xce\x45\x73\x23\x32\xee\x95\x76\x59\xaf\x6e\xfb\x4a\x58\x36\x76\xec"
         "\x71\xe4\xee\x51\x6e\x68\x9d\x37\x56\x06\xd1\x17\x9f\x22\xf5\x69\xb6"
         "\x7c\x4b\x47\x87\x4a\x14\xca\x1b\xed\xd4\xc2\x8f\xde\x9b\x28\x96\xc9"
         "\x0e\xd8\xac\xaa\xcc\x50\xb1\xba\x79\xa2\x7c\xe2\xf8\x6f\x33\x56\xd9"
         "\xda\xb1\xec\x5a\xe7\xb7\x1c\x64\xe5\x42\xc4\x57\x85\x7f\xb2\xdd\x53"
         "\xc5\xe1\xfc\x9b\xde\xf0\x07\xdb\xe5\xfb\x55\xe0\x72\x3f\x4f\x44\xf0"
         "\x97\x1d\xf3\x24\xbf\x3e\x00\x51\x35\xf4\x4e\xc0\x1c\xf1\x19\xc7\x31"
         "\x40\x22\x82\x5c\xfe\xfc\xca\x63\xda\xbc\x87\xc8\x2d\xe5\xc6\x5a\x09"
         "\xeb\x9e\xee\xf2\xc8\xa9\xb2\x1d\x9e\xd0\x64\x61\xdc\x21\xec\xd2\xc2"
         "\x15\x70\x53\x9e\x60\x20\x75\x68\x02\xdd\x0f\x04\x61\xb4\xcf\x7d\xdb",
         493);
  *(uint64_t*)0x20000048 = 0xfc;
  *(uint64_t*)0x20000218 = 1;
  *(uint64_t*)0x20000220 = 0;
  *(uint64_t*)0x20000228 = 0;
  *(uint32_t*)0x20000230 = 0x4000000;
  syscall(__NR_sendmsg, r[0], 0x20000200ul, 0x940ul);
  return 0;
}