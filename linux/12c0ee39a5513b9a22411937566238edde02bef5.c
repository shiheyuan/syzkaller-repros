// BUG: unable to handle kernel NULL pointer dereference in cipso_v4_sock_setattr
// https://syzkaller.appspot.com/bug?id=12c0ee39a5513b9a22411937566238edde02bef5
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
  res = syscall(__NR_socket, 2ul, 1ul, 0x106ul);
  if (res != -1)
    r[0] = res;
  memcpy((void*)0x200001c0, "security.SMACK64IPOUT\000", 22);
  memcpy((void*)0x20000200, "batadv0\000", 8);
  *(uint8_t*)0x20000208 = 0;
  syscall(__NR_fsetxattr, r[0], 0x200001c0ul, 0x20000200ul, 9ul, 3ul);
  return 0;
}