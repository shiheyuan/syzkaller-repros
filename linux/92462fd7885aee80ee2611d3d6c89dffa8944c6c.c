// KMSAN: uninit-value in rawv6_sendmsg
// https://syzkaller.appspot.com/bug?id=92462fd7885aee80ee2611d3d6c89dffa8944c6c
// status:invalid
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
  res = syscall(__NR_socket, 0xa, 3, 0x3a);
  if (res != -1)
    r[0] = res;
  *(uint16_t*)0x20002fe4 = 0xa;
  *(uint16_t*)0x20002fe6 = htobe16(0);
  *(uint32_t*)0x20002fe8 = 0;
  *(uint8_t*)0x20002fec = 0;
  *(uint8_t*)0x20002fed = 0;
  *(uint8_t*)0x20002fee = 0;
  *(uint8_t*)0x20002fef = 0;
  *(uint8_t*)0x20002ff0 = 0;
  *(uint8_t*)0x20002ff1 = 0;
  *(uint8_t*)0x20002ff2 = 0;
  *(uint8_t*)0x20002ff3 = 0;
  *(uint8_t*)0x20002ff4 = 0;
  *(uint8_t*)0x20002ff5 = 0;
  *(uint8_t*)0x20002ff6 = 0;
  *(uint8_t*)0x20002ff7 = 0;
  *(uint8_t*)0x20002ff8 = 0;
  *(uint8_t*)0x20002ff9 = 0;
  *(uint8_t*)0x20002ffa = 0;
  *(uint8_t*)0x20002ffb = 0;
  *(uint32_t*)0x20002ffc = 0;
  syscall(__NR_connect, r[0], 0x20002fe4, 0x1c);
  *(uint64_t*)0x20007000 = 0;
  *(uint32_t*)0x20007008 = 0;
  *(uint64_t*)0x20007010 = 0x20000ff0;
  *(uint64_t*)0x20000ff0 = 0x20000140;
  memcpy((void*)0x20000140, "\x02\x00", 2);
  *(uint64_t*)0x20000ff8 = 2;
  *(uint64_t*)0x20007018 = 1;
  *(uint64_t*)0x20007020 = 0x20003000;
  *(uint64_t*)0x20007028 = 0;
  *(uint32_t*)0x20007030 = 0;
  syscall(__NR_sendmsg, r[0], 0x20007000, 0x2000c080);
  memcpy((void*)0x200001c0, "\x7f\x45\x4c\x46\x80\x00\x00\x00\x00\x00\x00\x00"
                            "\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                            "\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00"
                            "\x00\x00\x17\x6d\x30\x4e\x00\x00\x00\x00",
         46);
  syscall(__NR_write, r[0], 0x200001c0, 0x2e);
  res = syscall(__NR_socket, 2, 1, 0);
  if (res != -1)
    r[1] = res;
  *(uint32_t*)0x20356ffc = 0xffffff40;
  syscall(__NR_setsockopt, r[1], 1, 0xf, 0x20356ffc, 4);
  *(uint16_t*)0x20000040 = 2;
  *(uint16_t*)0x20000042 = htobe16(0x4e20);
  *(uint32_t*)0x20000044 = htobe32(0);
  *(uint8_t*)0x20000048 = 0;
  *(uint8_t*)0x20000049 = 0;
  *(uint8_t*)0x2000004a = 0;
  *(uint8_t*)0x2000004b = 0;
  *(uint8_t*)0x2000004c = 0;
  *(uint8_t*)0x2000004d = 0;
  *(uint8_t*)0x2000004e = 0;
  *(uint8_t*)0x2000004f = 0;
  syscall(__NR_bind, r[1], 0x20000040, 0x10);
  syscall(__NR_listen, r[1], 0);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}