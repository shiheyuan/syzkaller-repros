// general protection fault in nf_tables_set_lookup
// https://syzkaller.appspot.com/bug?id=4a20ee2014cb8519da205bd85c0516dbd63fcce3
// status:open
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

uint64_t r[1] = {0xffffffffffffffff};
void loop()
{
  long res = 0;
  res = syscall(__NR_socket, 0x10, 3, 0xc);
  if (res != -1)
    r[0] = res;
  *(uint64_t*)0x20000040 = 0x20007340;
  *(uint16_t*)0x20007340 = 0x10;
  *(uint16_t*)0x20007342 = 0;
  *(uint32_t*)0x20007344 = 0;
  *(uint32_t*)0x20007348 = 0;
  *(uint32_t*)0x20000048 = 0xc;
  *(uint64_t*)0x20000050 = 0x20000080;
  *(uint64_t*)0x20000080 = 0x20004c80;
  *(uint32_t*)0x20004c80 = 0xb0;
  *(uint8_t*)0x20004c84 = 0xa;
  *(uint8_t*)0x20004c85 = 0xa;
  *(uint16_t*)0x20004c86 = 1;
  *(uint32_t*)0x20004c88 = 0;
  *(uint32_t*)0x20004c8c = 0;
  *(uint8_t*)0x20004c90 = 2;
  *(uint8_t*)0x20004c91 = 0;
  *(uint16_t*)0x20004c92 = htobe16(0xfffe);
  *(uint16_t*)0x20004c94 = 8;
  *(uint16_t*)0x20004c96 = 2;
  *(uint16_t*)0x20004c98 = 4;
  *(uint16_t*)0x20004c9a = 0x41;
  *(uint16_t*)0x20004c9c = 0x28;
  *(uint16_t*)0x20004c9e = 0x8f;
  *(uint16_t*)0x20004ca0 = 8;
  *(uint16_t*)0x20004ca2 = 0x8b;
  *(uint32_t*)0x20004ca4 = 0;
  *(uint16_t*)0x20004ca8 = 8;
  *(uint16_t*)0x20004caa = 0x43;
  *(uint32_t*)0x20004cac = 1;
  *(uint16_t*)0x20004cb0 = 0x14;
  *(uint16_t*)0x20004cb2 = 0x6a;
  *(uint8_t*)0x20004cb4 = 0xfe;
  *(uint8_t*)0x20004cb5 = 0x80;
  *(uint8_t*)0x20004cb6 = 0;
  *(uint8_t*)0x20004cb7 = 0;
  *(uint8_t*)0x20004cb8 = 0;
  *(uint8_t*)0x20004cb9 = 0;
  *(uint8_t*)0x20004cba = 0;
  *(uint8_t*)0x20004cbb = 0;
  *(uint8_t*)0x20004cbc = 0;
  *(uint8_t*)0x20004cbd = 0;
  *(uint8_t*)0x20004cbe = 0;
  *(uint8_t*)0x20004cbf = 0;
  *(uint8_t*)0x20004cc0 = 0;
  *(uint8_t*)0x20004cc1 = 0;
  *(uint8_t*)0x20004cc2 = 0;
  *(uint8_t*)0x20004cc3 = 0xaa;
  *(uint16_t*)0x20004cc4 = 0x18;
  *(uint16_t*)0x20004cc6 = 0x17;
  *(uint16_t*)0x20004cc8 = 8;
  *(uint16_t*)0x20004cca = 0x65;
  *(uint8_t*)0x20004ccc = 0xac;
  *(uint8_t*)0x20004ccd = 0x14;
  *(uint8_t*)0x20004cce = 0x14;
  *(uint8_t*)0x20004ccf = 0x1f;
  *(uint16_t*)0x20004cd0 = 8;
  *(uint16_t*)0x20004cd2 = 0x3f;
  *(uint32_t*)0x20004cd4 = 0;
  *(uint16_t*)0x20004cd8 = 4;
  *(uint16_t*)0x20004cda = 0xd;
  *(uint16_t*)0x20004cdc = 0x14;
  *(uint16_t*)0x20004cde = 0x22;
  *(uint16_t*)0x20004ce0 = 8;
  *(uint16_t*)0x20004ce2 = 0x2d;
  memcpy((void*)0x20004ce4, "", 1);
  *(uint16_t*)0x20004ce8 = 8;
  *(uint16_t*)0x20004cea = 0x41;
  *(uint32_t*)0x20004cec = 0;
  *(uint16_t*)0x20004cf0 = 0x14;
  *(uint16_t*)0x20004cf2 = 0x2b;
  *(uint16_t*)0x20004cf4 = 8;
  *(uint16_t*)0x20004cf6 = 0x42;
  *(uint32_t*)0x20004cf8 = htobe32(-1);
  *(uint16_t*)0x20004cfc = 8;
  *(uint16_t*)0x20004cfe = 0x65;
  *(uint32_t*)0x20004d00 = 0;
  *(uint16_t*)0x20004d04 = 0x18;
  *(uint16_t*)0x20004d06 = 0x25;
  *(uint16_t*)0x20004d08 = 8;
  *(uint16_t*)0x20004d0a = 0x1c;
  memcpy((void*)0x20004d0c, "", 1);
  *(uint16_t*)0x20004d10 = 8;
  *(uint16_t*)0x20004d12 = 0x5a;
  *(uint32_t*)0x20004d14 = 0;
  *(uint16_t*)0x20004d18 = 4;
  *(uint16_t*)0x20004d1a = 0x3b;
  *(uint16_t*)0x20004d1c = 0x14;
  *(uint16_t*)0x20004d1e = 0x52;
  *(uint16_t*)0x20004d20 = 8;
  *(uint16_t*)0x20004d22 = 0x1b;
  *(uint32_t*)0x20004d24 = htobe32(0xe0000001);
  *(uint16_t*)0x20004d28 = 8;
  *(uint16_t*)0x20004d2a = 0x79;
  *(uint32_t*)0x20004d2c = 0x3ff;
  *(uint64_t*)0x20000088 = 0xb0;
  *(uint64_t*)0x20000058 = 1;
  *(uint64_t*)0x20000060 = 0;
  *(uint64_t*)0x20000068 = 0;
  *(uint32_t*)0x20000070 = 0x4004010;
  syscall(__NR_sendmsg, r[0], 0x20000040, 0);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}