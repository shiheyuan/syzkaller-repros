// BUG: unable to handle kernel NULL pointer dereference in smc_setsockopt
// https://syzkaller.appspot.com/bug?id=7f44c2326ef4b1ca17f14c21fc9ef04a257e911d
// status:invalid
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

uint64_t r[1] = {0xffffffffffffffff};
void loop()
{
  long res = 0;
  res = syscall(__NR_socket, 0x2b, 1, 0);
  if (res != -1)
    r[0] = res;
  syscall(__NR_listen, r[0], 1);
  syscall(__NR_shutdown, r[0], 0);
  *(uint32_t*)0x200012c0 = 0;
  *(uint32_t*)0x200012c4 = 0x3f;
  syscall(__NR_setsockopt, r[0], 0x84, 0x75, 0x200012c0, 8);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}
