// INFO: task hung in snd_seq_do_ioctl
// https://syzkaller.appspot.com/bug?id=9e4a19095ebae54877fbd1758779d99e0e9c4a86
// status:open
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

long r[1];
void loop()
{
  memset(r, -1, sizeof(r));
  syscall(__NR_mmap, 0x20000000, 0xfff000, 3, 0x32, -1, 0);
  memcpy((void*)0x205a8000, "/dev/sequencer2", 16);
  r[0] = syscall(__NR_openat, 0xffffffffffffff9c, 0x205a8000, 0x8002, 0);
  *(uint8_t*)0x2000a000 = 0x81;
  *(uint8_t*)0x2000a001 = 6;
  *(uint8_t*)0x2000a002 = 0;
  *(uint8_t*)0x2000a003 = 0;
  *(uint64_t*)0x2000a008 = 0x77359400;
  *(uint64_t*)0x2000a010 = 0;
  *(uint8_t*)0x2000a018 = 0;
  *(uint8_t*)0x2000a019 = 0;
  *(uint8_t*)0x2000a01a = 0;
  *(uint8_t*)0x2000a01b = 0;
  *(uint32_t*)0x2000a020 = 0;
  syscall(__NR_write, r[0], 0x2000a000, 0x30);
}

int main()
{
  loop();
  return 0;
}
