// panic: munmap(ADDR, 0)) failed: invalid argument
// https://syzkaller.appspot.com/bug?id=3d5bf4f088efbffff3fc7a2063c7be663101b2e5
// status:fixed
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

void loop()
{
  syscall(__NR_mremap, 0x2000d000, 0xfffffffffffffe74, 0x1000, 3, 0x20007000);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}