// BUG: using __this_cpu_read() in preemptible [ADDR] code: syzkaller5NUM/3348
// https://syzkaller.appspot.com/bug?id=f0f6ac84d82516c544f7bafb34d062b4630963d3
// status:invalid
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <sys/syscall.h>
#include <unistd.h>

#include <stdint.h>
#include <string.h>

static void test();

void loop()
{
  while (1) {
    test();
  }
}

#ifndef __NR_mmap
#define __NR_mmap 192
#endif
#ifndef __NR_socket
#define __NR_socket 359
#endif
#ifndef __NR_sendmsg
#define __NR_sendmsg 370
#endif
#undef __NR_mmap
#define __NR_mmap __NR_mmap2

long r[60];
void test()
{
  memset(r, -1, sizeof(r));
  r[0] = syscall(__NR_mmap, 0x20000000ul, 0xf7a000ul, 0x3ul, 0x32ul,
                 0xfffffffffffffffful, 0x0ul);
  r[1] = syscall(__NR_socket, 0xful, 0x3ul, 0x2ul);
  *(uint32_t*)0x20900000 = (uint32_t)0x0;
  *(uint32_t*)0x20900004 = (uint32_t)0x0;
  *(uint32_t*)0x20900008 = (uint32_t)0x208feff0;
  *(uint32_t*)0x2090000c = (uint32_t)0x1;
  *(uint32_t*)0x20900010 = (uint32_t)0x0;
  *(uint32_t*)0x20900014 = (uint32_t)0x0;
  *(uint32_t*)0x20900018 = (uint32_t)0x0;
  *(uint32_t*)0x208feff0 = (uint32_t)0x208fe000;
  *(uint32_t*)0x208feff4 = (uint32_t)0x50;
  *(uint8_t*)0x208fe000 = (uint8_t)0x2;
  *(uint8_t*)0x208fe001 = (uint8_t)0x2;
  *(uint8_t*)0x208fe002 = (uint8_t)0x0;
  *(uint8_t*)0x208fe003 = (uint8_t)0x9;
  *(uint16_t*)0x208fe004 = (uint16_t)0xa;
  *(uint16_t*)0x208fe006 = (uint16_t)0x0;
  *(uint32_t*)0x208fe008 = (uint32_t)0x0;
  *(uint32_t*)0x208fe00c = (uint32_t)0x0;
  *(uint16_t*)0x208fe010 = (uint16_t)0x3;
  *(uint16_t*)0x208fe012 = (uint16_t)0x80006;
  *(uint8_t*)0x208fe014 = (uint8_t)0x0;
  *(uint8_t*)0x208fe015 = (uint8_t)0x0;
  *(uint16_t*)0x208fe016 = (uint16_t)0x0;
  *(uint16_t*)0x208fe018 = (uint16_t)0x2;
  *(uint16_t*)0x208fe01a = (uint16_t)0x204e;
  *(uint32_t*)0x208fe01c = (uint32_t)0x10000e0;
  *(uint8_t*)0x208fe020 = (uint8_t)0x0;
  *(uint8_t*)0x208fe021 = (uint8_t)0x0;
  *(uint8_t*)0x208fe022 = (uint8_t)0x0;
  *(uint8_t*)0x208fe023 = (uint8_t)0x0;
  *(uint8_t*)0x208fe024 = (uint8_t)0x0;
  *(uint8_t*)0x208fe025 = (uint8_t)0x0;
  *(uint8_t*)0x208fe026 = (uint8_t)0x0;
  *(uint8_t*)0x208fe027 = (uint8_t)0x0;
  *(uint16_t*)0x208fe028 = (uint16_t)0x2;
  *(uint16_t*)0x208fe02a = (uint16_t)0x1;
  *(uint32_t*)0x208fe02c = (uint32_t)0xfcffffff;
  *(uint8_t*)0x208fe030 = (uint8_t)0x0;
  *(uint8_t*)0x208fe031 = (uint8_t)0x0;
  *(uint8_t*)0x208fe032 = (uint8_t)0x0;
  *(uint8_t*)0x208fe033 = (uint8_t)0x2;
  *(uint32_t*)0x208fe034 = (uint32_t)0x0;
  *(uint16_t*)0x208fe038 = (uint16_t)0x3;
  *(uint16_t*)0x208fe03a = (uint16_t)0x5;
  *(uint8_t*)0x208fe03c = (uint8_t)0x0;
  *(uint8_t*)0x208fe03d = (uint8_t)0x0;
  *(uint16_t*)0x208fe03e = (uint16_t)0x0;
  *(uint16_t*)0x208fe040 = (uint16_t)0x2;
  *(uint16_t*)0x208fe042 = (uint16_t)0x214e;
  *(uint32_t*)0x208fe044 = (uint32_t)0x100007f;
  *(uint8_t*)0x208fe048 = (uint8_t)0x0;
  *(uint8_t*)0x208fe049 = (uint8_t)0x0;
  *(uint8_t*)0x208fe04a = (uint8_t)0x0;
  *(uint8_t*)0x208fe04b = (uint8_t)0x0;
  *(uint8_t*)0x208fe04c = (uint8_t)0x0;
  *(uint8_t*)0x208fe04d = (uint8_t)0x0;
  *(uint8_t*)0x208fe04e = (uint8_t)0x0;
  *(uint8_t*)0x208fe04f = (uint8_t)0x0;
  r[59] = syscall(__NR_sendmsg, r[1], 0x20900000ul, 0x0ul);
}

int main()
{
  int i;
  for (i = 0; i < 8; i++) {
    if (fork() == 0) {
      loop();
      return 0;
    }
  }
  sleep(1000000);
  return 0;
}