// KASAN: global-out-of-bounds Read in crypto_chacha20_crypt
// https://syzkaller.appspot.com/bug?id=d841d0560a2b94da20b7330b7941bb5f52daebcb
// status:fixed
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

long r[2];
void test()
{
  memset(r, -1, sizeof(r));
  syscall(__NR_mmap, 0x20000000ul, 0xf7b000ul, 0x3ul, 0x32ul,
          0xfffffffffffffffful, 0x0ul);
  r[0] = syscall(__NR_socket, 0x26ul, 0x5ul, 0x0ul);
  *(uint16_t*)0x20f75fa8 = (uint16_t)0x26;
  memcpy((void*)0x20f75faa,
         "\x73\x6b\x63\x69\x70\x68\x65\x72\x00\x00\x00\x00\x00\x00",
         14);
  *(uint32_t*)0x20f75fb8 = (uint32_t)0x0;
  *(uint32_t*)0x20f75fbc = (uint32_t)0x0;
  memcpy((void*)0x20f75fc0,
         "\x63\x72\x79\x70\x74\x64\x28\x63\x68\x61\x63\x68\x61\x32\x30"
         "\x2d\x73\x69\x6d\x64\x29\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00",
         64);
  syscall(__NR_bind, r[0], 0x20f75fa8ul, 0x58ul);
  memcpy((void*)0x20b7a000, "\x0a\x07\x75\xb0\xd5\xe3\x83\xe5\xb3\xb6"
                            "\x0c\xed\x5c\x54\xdb\xb7\x29\x5d\xf0\xdf"
                            "\x82\x17\xad\x40\x00\x00\x00\x00\x00\x00"
                            "\x00\xe6",
         32);
  syscall(__NR_setsockopt, r[0], 0x117ul, 0x1ul, 0x20b7a000ul, 0x20ul);
  r[1] = syscall(__NR_accept, r[0], 0x0ul, 0x0ul);
  *(uint64_t*)0x201f5000 = (uint64_t)0x20a06000;
  *(uint32_t*)0x201f5008 = (uint32_t)0x58;
  *(uint64_t*)0x201f5010 = (uint64_t)0x20233fd0;
  *(uint64_t*)0x201f5018 = (uint64_t)0x2;
  *(uint64_t*)0x201f5020 = (uint64_t)0x208e5fa9;
  *(uint64_t*)0x201f5028 = (uint64_t)0x0;
  *(uint32_t*)0x201f5030 = (uint32_t)0x8a;
  *(uint64_t*)0x20233fd0 = (uint64_t)0x20be4ffb;
  *(uint64_t*)0x20233fd8 = (uint64_t)0x0;
  *(uint64_t*)0x20233fe0 = (uint64_t)0x20f71fc6;
  *(uint64_t*)0x20233fe8 = (uint64_t)0x3a;
  syscall(__NR_recvmsg, r[1], 0x201f5000ul, 0x20ul);
}

int main()
{
  loop();
  return 0;
}