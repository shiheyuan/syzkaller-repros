// BUG: using __this_cpu_read() in preemptible [ADDR] code: syzkaller3NUM/3307
// https://syzkaller.appspot.com/bug?id=3d2aaab1c23ee1761d0e85088d80a36bfdc9922a
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

long r[63];
void test()
{
  memset(r, -1, sizeof(r));
  r[0] = syscall(__NR_mmap, 0x20000000ul, 0xfff000ul, 0x3ul, 0x32ul,
                 0xfffffffffffffffful, 0x0ul);
  r[1] = syscall(__NR_socket, 0xful, 0x3ul, 0x2ul);
  *(uint32_t*)0x205f5000 = (uint32_t)0x0;
  *(uint32_t*)0x205f5004 = (uint32_t)0x0;
  *(uint32_t*)0x205f5008 = (uint32_t)0x208feff0;
  *(uint32_t*)0x205f500c = (uint32_t)0x1;
  *(uint32_t*)0x205f5010 = (uint32_t)0x0;
  *(uint32_t*)0x205f5014 = (uint32_t)0x0;
  *(uint32_t*)0x205f5018 = (uint32_t)0x0;
  *(uint32_t*)0x208feff0 = (uint32_t)0x20400fb0;
  *(uint32_t*)0x208feff4 = (uint32_t)0x50;
  *(uint8_t*)0x20400fb0 = (uint8_t)0x2;
  *(uint8_t*)0x20400fb1 = (uint8_t)0x3;
  *(uint8_t*)0x20400fb2 = (uint8_t)0x0;
  *(uint8_t*)0x20400fb3 = (uint8_t)0x9;
  *(uint16_t*)0x20400fb4 = (uint16_t)0xa;
  *(uint16_t*)0x20400fb6 = (uint16_t)0x0;
  *(uint32_t*)0x20400fb8 = (uint32_t)0x3;
  *(uint32_t*)0x20400fbc = (uint32_t)0x0;
  *(uint16_t*)0x20400fc0 = (uint16_t)0x3;
  *(uint16_t*)0x20400fc2 = (uint16_t)0x6;
  *(uint8_t*)0x20400fc4 = (uint8_t)0x0;
  *(uint8_t*)0x20400fc5 = (uint8_t)0x0;
  *(uint16_t*)0x20400fc6 = (uint16_t)0x0;
  *(uint16_t*)0x20400fc8 = (uint16_t)0x2;
  *(uint16_t*)0x20400fca = (uint16_t)0x204e;
  *(uint32_t*)0x20400fcc = (uint32_t)0x1020000;
  *(uint8_t*)0x20400fd0 = (uint8_t)0x0;
  *(uint8_t*)0x20400fd1 = (uint8_t)0x0;
  *(uint8_t*)0x20400fd2 = (uint8_t)0x0;
  *(uint8_t*)0x20400fd3 = (uint8_t)0x0;
  *(uint8_t*)0x20400fd4 = (uint8_t)0x0;
  *(uint8_t*)0x20400fd5 = (uint8_t)0x0;
  *(uint8_t*)0x20400fd6 = (uint8_t)0x0;
  *(uint8_t*)0x20400fd7 = (uint8_t)0x0;
  *(uint16_t*)0x20400fd8 = (uint16_t)0x2;
  *(uint16_t*)0x20400fda = (uint16_t)0x1;
  *(uint32_t*)0x20400fdc = (uint32_t)0x0;
  *(uint8_t*)0x20400fe0 = (uint8_t)0x0;
  *(uint8_t*)0x20400fe1 = (uint8_t)0x8000000000;
  *(uint8_t*)0x20400fe2 = (uint8_t)0x0;
  *(uint8_t*)0x20400fe3 = (uint8_t)0x2;
  *(uint32_t*)0x20400fe4 = (uint32_t)0x10001;
  *(uint16_t*)0x20400fe8 = (uint16_t)0x3;
  *(uint16_t*)0x20400fea = (uint16_t)0x2000040005;
  *(uint8_t*)0x20400fec = (uint8_t)0x0;
  *(uint8_t*)0x20400fed = (uint8_t)0x0;
  *(uint16_t*)0x20400fee = (uint16_t)0x0;
  *(uint16_t*)0x20400ff0 = (uint16_t)0x2;
  *(uint16_t*)0x20400ff2 = (uint16_t)0x204e;
  *(uint8_t*)0x20400ff4 = (uint8_t)0xac;
  *(uint8_t*)0x20400ff5 = (uint8_t)0x14;
  *(uint8_t*)0x20400ff6 = (uint8_t)0x0;
  *(uint8_t*)0x20400ff7 = (uint8_t)0xaa;
  *(uint8_t*)0x20400ff8 = (uint8_t)0x0;
  *(uint8_t*)0x20400ff9 = (uint8_t)0x0;
  *(uint8_t*)0x20400ffa = (uint8_t)0x0;
  *(uint8_t*)0x20400ffb = (uint8_t)0x0;
  *(uint8_t*)0x20400ffc = (uint8_t)0x0;
  *(uint8_t*)0x20400ffd = (uint8_t)0x0;
  *(uint8_t*)0x20400ffe = (uint8_t)0x0;
  *(uint8_t*)0x20400fff = (uint8_t)0x0;
  r[62] = syscall(__NR_sendmsg, r[1], 0x205f5000ul, 0x0ul);
}

int main()
{
  loop();
  return 0;
}
