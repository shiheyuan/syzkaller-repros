// KASAN: slab-out-of-bounds Write in sha3_final
// https://syzkaller.appspot.com/bug?id=51646eda41a14c8061c2af11bccb3d260f1e7d9e
// status:dup
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <pthread.h>
#include <stdlib.h>
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

long r[148];
void* thr(void* arg)
{
  switch ((long)arg) {
  case 0:
    r[0] = syscall(__NR_mmap, 0x20000000ul, 0xfff000ul, 0x3ul, 0x32ul,
                   0xfffffffffffffffful, 0x0ul);
    break;
  case 1:
    r[1] = syscall(__NR_socket, 0x26ul, 0x5ul, 0x0ul);
    break;
  case 2:
    *(uint16_t*)0x209d9000 = (uint16_t)0x26;
    memcpy((void*)0x209d9002,
           "\x68\x61\x73\x68\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
           14);
    *(uint32_t*)0x209d9010 = (uint32_t)0x0;
    *(uint32_t*)0x209d9014 = (uint32_t)0x0;
    memcpy((void*)0x209d9018,
           "\x68\x6d\x61\x63\x28\x68\x6d\x61\x63\x28\x73\x68\x61\x33"
           "\x2d\x35\x31\x32\x2d\x67\x65\x6e\x65\x72\x69\x63\x29\x29"
           "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
           "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
           "\x00\x00\x00\x00\x00\x00\x00\x00",
           64);
    r[7] = syscall(__NR_bind, r[1], 0x209d9000ul, 0x58ul);
    break;
  case 3:
    r[8] = syscall(__NR_setsockopt, r[1], 0x117ul, 0x1ul, 0x209d2ff2ul,
                   0x0ul);
    break;
  case 4:
    r[9] = syscall(__NR_accept, r[1], 0x0ul, 0x0ul);
    break;
  case 5:
    *(uint64_t*)0x209fe000 = (uint64_t)0x0;
    *(uint32_t*)0x209fe008 = (uint32_t)0x0;
    *(uint64_t*)0x209fe010 = (uint64_t)0x207a6fd0;
    *(uint64_t*)0x209fe018 = (uint64_t)0x3;
    *(uint64_t*)0x209fe020 = (uint64_t)0x20566fa0;
    *(uint64_t*)0x209fe028 = (uint64_t)0x60;
    *(uint32_t*)0x209fe030 = (uint32_t)0x1;
    *(uint64_t*)0x209fe038 = (uint64_t)0x0;
    *(uint32_t*)0x209fe040 = (uint32_t)0x0;
    *(uint64_t*)0x209fe048 = (uint64_t)0x20284fe0;
    *(uint64_t*)0x209fe050 = (uint64_t)0x2;
    *(uint64_t*)0x209fe058 = (uint64_t)0x2034bf58;
    *(uint64_t*)0x209fe060 = (uint64_t)0x60;
    *(uint32_t*)0x209fe068 = (uint32_t)0xc080;
    *(uint64_t*)0x209fe070 = (uint64_t)0x0;
    *(uint32_t*)0x209fe078 = (uint32_t)0x0;
    *(uint64_t*)0x209fe080 = (uint64_t)0x20296000;
    *(uint64_t*)0x209fe088 = (uint64_t)0x6;
    *(uint64_t*)0x209fe090 = (uint64_t)0x2056e000;
    *(uint64_t*)0x209fe098 = (uint64_t)0x90;
    *(uint32_t*)0x209fe0a0 = (uint32_t)0x1;
    *(uint64_t*)0x209fe0a8 = (uint64_t)0x0;
    *(uint32_t*)0x209fe0b0 = (uint32_t)0x0;
    *(uint64_t*)0x209fe0b8 = (uint64_t)0x20b9e000;
    *(uint64_t*)0x209fe0c0 = (uint64_t)0x3;
    *(uint64_t*)0x209fe0c8 = (uint64_t)0x2084a000;
    *(uint64_t*)0x209fe0d0 = (uint64_t)0x0;
    *(uint32_t*)0x209fe0d8 = (uint32_t)0x4000000;
    *(uint64_t*)0x209fe0e0 = (uint64_t)0x0;
    *(uint32_t*)0x209fe0e8 = (uint32_t)0x0;
    *(uint64_t*)0x209fe0f0 = (uint64_t)0x2091e000;
    *(uint64_t*)0x209fe0f8 = (uint64_t)0x1;
    *(uint64_t*)0x209fe100 = (uint64_t)0x2004efa0;
    *(uint64_t*)0x209fe108 = (uint64_t)0x60;
    *(uint32_t*)0x209fe110 = (uint32_t)0x20000000;
    *(uint64_t*)0x207a6fd0 = (uint64_t)0x2024a000;
    *(uint64_t*)0x207a6fd8 = (uint64_t)0x0;
    *(uint64_t*)0x207a6fe0 = (uint64_t)0x20a05000;
    *(uint64_t*)0x207a6fe8 = (uint64_t)0x0;
    *(uint64_t*)0x207a6ff0 = (uint64_t)0x20e14000;
    *(uint64_t*)0x207a6ff8 = (uint64_t)0x0;
    *(uint64_t*)0x20566fa0 = (uint64_t)0x18;
    *(uint32_t*)0x20566fa8 = (uint32_t)0x117;
    *(uint32_t*)0x20566fac = (uint32_t)0x3;
    *(uint32_t*)0x20566fb0 = (uint32_t)0x0;
    *(uint64_t*)0x20566fb8 = (uint64_t)0x18;
    *(uint32_t*)0x20566fc0 = (uint32_t)0x117;
    *(uint32_t*)0x20566fc4 = (uint32_t)0x3;
    *(uint32_t*)0x20566fc8 = (uint32_t)0x1;
    *(uint64_t*)0x20566fd0 = (uint64_t)0x18;
    *(uint32_t*)0x20566fd8 = (uint32_t)0x117;
    *(uint32_t*)0x20566fdc = (uint32_t)0x4;
    *(uint32_t*)0x20566fe0 = (uint32_t)0x7;
    *(uint64_t*)0x20566fe8 = (uint64_t)0x18;
    *(uint32_t*)0x20566ff0 = (uint32_t)0x117;
    *(uint32_t*)0x20566ff4 = (uint32_t)0x4;
    *(uint32_t*)0x20566ff8 = (uint32_t)0x6;
    *(uint64_t*)0x20284fe0 = (uint64_t)0x200fc000;
    *(uint64_t*)0x20284fe8 = (uint64_t)0x0;
    *(uint64_t*)0x20284ff0 = (uint64_t)0x20df2ffd;
    *(uint64_t*)0x20284ff8 = (uint64_t)0x0;
    *(uint64_t*)0x2034bf58 = (uint64_t)0x18;
    *(uint32_t*)0x2034bf60 = (uint32_t)0x117;
    *(uint32_t*)0x2034bf64 = (uint32_t)0x3;
    *(uint32_t*)0x2034bf68 = (uint32_t)0x0;
    *(uint64_t*)0x2034bf70 = (uint64_t)0x18;
    *(uint32_t*)0x2034bf78 = (uint32_t)0x117;
    *(uint32_t*)0x2034bf7c = (uint32_t)0x3;
    *(uint32_t*)0x2034bf80 = (uint32_t)0x1;
    *(uint64_t*)0x2034bf88 = (uint64_t)0x18;
    *(uint32_t*)0x2034bf90 = (uint32_t)0x117;
    *(uint32_t*)0x2034bf94 = (uint32_t)0x2;
    *(uint32_t*)0x2034bf98 = (uint32_t)0x0;
    *(uint64_t*)0x2034bfa0 = (uint64_t)0x18;
    *(uint32_t*)0x2034bfa8 = (uint32_t)0x117;
    *(uint32_t*)0x2034bfac = (uint32_t)0x2;
    *(uint32_t*)0x2034bfb0 = (uint32_t)0x0;
    *(uint64_t*)0x20296000 = (uint64_t)0x20ef1000;
    *(uint64_t*)0x20296008 = (uint64_t)0x0;
    *(uint64_t*)0x20296010 = (uint64_t)0x20ca0000;
    *(uint64_t*)0x20296018 = (uint64_t)0x0;
    *(uint64_t*)0x20296020 = (uint64_t)0x20ce0000;
    *(uint64_t*)0x20296028 = (uint64_t)0x0;
    *(uint64_t*)0x20296030 = (uint64_t)0x208f6fe5;
    *(uint64_t*)0x20296038 = (uint64_t)0x0;
    *(uint64_t*)0x20296040 = (uint64_t)0x20d16000;
    *(uint64_t*)0x20296048 = (uint64_t)0x0;
    *(uint64_t*)0x20296050 = (uint64_t)0x20ccbfe6;
    *(uint64_t*)0x20296058 = (uint64_t)0x0;
    *(uint64_t*)0x2056e000 = (uint64_t)0x18;
    *(uint32_t*)0x2056e008 = (uint32_t)0x117;
    *(uint32_t*)0x2056e00c = (uint32_t)0x2;
    *(uint32_t*)0x2056e010 = (uint32_t)0x0;
    *(uint64_t*)0x2056e018 = (uint64_t)0x18;
    *(uint32_t*)0x2056e020 = (uint32_t)0x117;
    *(uint32_t*)0x2056e024 = (uint32_t)0x4;
    *(uint32_t*)0x2056e028 = (uint32_t)0x800;
    *(uint64_t*)0x2056e030 = (uint64_t)0x18;
    *(uint32_t*)0x2056e038 = (uint32_t)0x117;
    *(uint32_t*)0x2056e03c = (uint32_t)0x2;
    *(uint32_t*)0x2056e040 = (uint32_t)0x0;
    *(uint64_t*)0x2056e048 = (uint64_t)0x18;
    *(uint32_t*)0x2056e050 = (uint32_t)0x117;
    *(uint32_t*)0x2056e054 = (uint32_t)0x4;
    *(uint32_t*)0x2056e058 = (uint32_t)0x1;
    *(uint64_t*)0x2056e060 = (uint64_t)0x18;
    *(uint32_t*)0x2056e068 = (uint32_t)0x117;
    *(uint32_t*)0x2056e06c = (uint32_t)0x4;
    *(uint32_t*)0x2056e070 = (uint32_t)0x100000000;
    *(uint64_t*)0x2056e078 = (uint64_t)0x18;
    *(uint32_t*)0x2056e080 = (uint32_t)0x117;
    *(uint32_t*)0x2056e084 = (uint32_t)0x3;
    *(uint32_t*)0x2056e088 = (uint32_t)0x1;
    *(uint64_t*)0x20b9e000 = (uint64_t)0x20d3d000;
    *(uint64_t*)0x20b9e008 = (uint64_t)0x0;
    *(uint64_t*)0x20b9e010 = (uint64_t)0x200f5000;
    *(uint64_t*)0x20b9e018 = (uint64_t)0x0;
    *(uint64_t*)0x20b9e020 = (uint64_t)0x20a6c000;
    *(uint64_t*)0x20b9e028 = (uint64_t)0x0;
    *(uint64_t*)0x2091e000 = (uint64_t)0x2018cffc;
    *(uint64_t*)0x2091e008 = (uint64_t)0x0;
    *(uint64_t*)0x2004efa0 = (uint64_t)0x18;
    *(uint32_t*)0x2004efa8 = (uint32_t)0x117;
    *(uint32_t*)0x2004efac = (uint32_t)0x2;
    *(uint32_t*)0x2004efb0 = (uint32_t)0x0;
    *(uint64_t*)0x2004efb8 = (uint64_t)0x18;
    *(uint32_t*)0x2004efc0 = (uint32_t)0x117;
    *(uint32_t*)0x2004efc4 = (uint32_t)0x2;
    *(uint32_t*)0x2004efc8 = (uint32_t)0x0;
    *(uint64_t*)0x2004efd0 = (uint64_t)0x18;
    *(uint32_t*)0x2004efd8 = (uint32_t)0x117;
    *(uint32_t*)0x2004efdc = (uint32_t)0x4;
    *(uint32_t*)0x2004efe0 = (uint32_t)0x1;
    *(uint64_t*)0x2004efe8 = (uint64_t)0x18;
    *(uint32_t*)0x2004eff0 = (uint32_t)0x117;
    *(uint32_t*)0x2004eff4 = (uint32_t)0x4;
    *(uint32_t*)0x2004eff8 = (uint32_t)0x104;
    r[147] = syscall(__NR_sendmmsg, r[9], 0x209fe000ul, 0x5ul, 0x0ul);
    break;
  }
  return 0;
}

void test()
{
  long i;
  pthread_t th[12];

  memset(r, -1, sizeof(r));
  for (i = 0; i < 6; i++) {
    pthread_create(&th[i], 0, thr, (void*)i);
    usleep(rand() % 10000);
  }
  usleep(rand() % 100000);
}

int main()
{
  loop();
  return 0;
}