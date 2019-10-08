// WARNING in reuseport_alloc
// https://syzkaller.appspot.com/bug?id=02a49e798ac084d20d84b358c92735fde98cc831
// status:fixed
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

long r[13];
void* thr(void* arg)
{
  switch ((long)arg) {
  case 0:
    r[0] = syscall(__NR_mmap, 0x20000000ul, 0xfff000ul, 0x3ul, 0x32ul,
                   0xfffffffffffffffful, 0x0ul);
    break;
  case 1:
    r[1] = syscall(__NR_socket, 0x2ul, 0x1ul, 0x0ul);
    break;
  case 2:
    *(uint32_t*)0x20f11000 = (uint32_t)0xffffffffffffffc0;
    r[3] = syscall(__NR_setsockopt, r[1], 0x1ul, 0xful, 0x20f11000ul,
                   0x4ul);
    break;
  case 3:
    *(uint16_t*)0x20e9bff0 = (uint16_t)0x1;
    *(uint64_t*)0x20e9bff8 = (uint64_t)0x20f07000;
    *(uint16_t*)0x20f07000 = (uint16_t)0x6;
    *(uint8_t*)0x20f07002 = (uint8_t)0x0;
    *(uint8_t*)0x20f07003 = (uint8_t)0x0;
    *(uint32_t*)0x20f07004 = (uint32_t)0x0;
    r[10] = syscall(__NR_setsockopt, r[1], 0x1ul, 0x33ul, 0x20e9bff0ul,
                    0x10ul);
    break;
  case 4:
    *(uint32_t*)0x200d1ffc = (uint32_t)0x5;
    r[12] = syscall(__NR_setsockopt, 0xfffffffffffffffful, 0x10eul,
                    0x4ul, 0x200d1ffcul, 0x4ul);
    break;
  }
  return 0;
}

void test()
{
  long i;
  pthread_t th[10];

  memset(r, -1, sizeof(r));
  srand(getpid());
  for (i = 0; i < 5; i++) {
    pthread_create(&th[i], 0, thr, (void*)i);
    usleep(rand() % 10000);
  }
  for (i = 0; i < 5; i++) {
    pthread_create(&th[5 + i], 0, thr, (void*)i);
    if (rand() % 2)
      usleep(rand() % 10000);
  }
  usleep(rand() % 100000);
}

int main()
{
  loop();
  return 0;
}