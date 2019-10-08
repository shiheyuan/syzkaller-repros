// KMSAN: uninit-value in put_cmsg
// https://syzkaller.appspot.com/bug?id=cb98d4633cad367e239e527e64850e1ddf489e70
// status:fixed
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <linux/futex.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

struct thread_t {
  int created, running, call;
  pthread_t th;
};

static struct thread_t threads[16];
static void execute_call(int call);
static int running;
static int collide;

static void* thr(void* arg)
{
  struct thread_t* th = (struct thread_t*)arg;
  for (;;) {
    while (!__atomic_load_n(&th->running, __ATOMIC_ACQUIRE))
      syscall(SYS_futex, &th->running, FUTEX_WAIT, 0, 0);
    execute_call(th->call);
    __atomic_fetch_sub(&running, 1, __ATOMIC_RELAXED);
    __atomic_store_n(&th->running, 0, __ATOMIC_RELEASE);
    syscall(SYS_futex, &th->running, FUTEX_WAKE);
  }
  return 0;
}

static void execute(int num_calls)
{
  int call, thread;
  running = 0;
  for (call = 0; call < num_calls; call++) {
    for (thread = 0; thread < sizeof(threads) / sizeof(threads[0]); thread++) {
      struct thread_t* th = &threads[thread];
      if (!th->created) {
        th->created = 1;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, 128 << 10);
        pthread_create(&th->th, &attr, thr, th);
      }
      if (!__atomic_load_n(&th->running, __ATOMIC_ACQUIRE)) {
        th->call = call;
        __atomic_fetch_add(&running, 1, __ATOMIC_RELAXED);
        __atomic_store_n(&th->running, 1, __ATOMIC_RELEASE);
        syscall(SYS_futex, &th->running, FUTEX_WAKE);
        if (collide && call % 2)
          break;
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 20 * 1000 * 1000;
        syscall(SYS_futex, &th->running, FUTEX_WAIT, 1, &ts);
        if (running)
          usleep((call == num_calls - 1) ? 10000 : 1000);
        break;
      }
    }
  }
}

uint64_t r[1] = {0xffffffffffffffff};
void execute_call(int call)
{
  long res;
  switch (call) {
  case 0:
    res = syscall(__NR_socket, 0x40000000015, 5, 0);
    if (res != -1)
      r[0] = res;
    break;
  case 1:
    *(uint16_t*)0x20fc4ff0 = 2;
    *(uint16_t*)0x20fc4ff2 = htobe16(0);
    *(uint32_t*)0x20fc4ff4 = htobe32(0x7f000001);
    *(uint8_t*)0x20fc4ff8 = 0;
    *(uint8_t*)0x20fc4ff9 = 0;
    *(uint8_t*)0x20fc4ffa = 0;
    *(uint8_t*)0x20fc4ffb = 0;
    *(uint8_t*)0x20fc4ffc = 0;
    *(uint8_t*)0x20fc4ffd = 0;
    *(uint8_t*)0x20fc4ffe = 0;
    *(uint8_t*)0x20fc4fff = 0;
    syscall(__NR_bind, r[0], 0x20fc4ff0, 0x10);
    break;
  case 2:
    memcpy((void*)0x203cbffc, "\x02\x00\x00\x00", 4);
    syscall(__NR_setsockopt, r[0], 0x114, 0xa, 0x203cbffc, 4);
    break;
  case 3:
    *(uint16_t*)0x20adf000 = 2;
    *(uint16_t*)0x20adf002 = htobe16(0);
    *(uint32_t*)0x20adf004 = htobe32(0x7f000001);
    *(uint8_t*)0x20adf008 = 0;
    *(uint8_t*)0x20adf009 = 0;
    *(uint8_t*)0x20adf00a = 0;
    *(uint8_t*)0x20adf00b = 0;
    *(uint8_t*)0x20adf00c = 0;
    *(uint8_t*)0x20adf00d = 0;
    *(uint8_t*)0x20adf00e = 0;
    *(uint8_t*)0x20adf00f = 0;
    syscall(__NR_connect, r[0], 0x20adf000, 0x10);
    break;
  case 4:
    *(uint64_t*)0x20000780 = 0x20000180;
    *(uint16_t*)0x20000180 = 3;
    memcpy((void*)0x20000182, "\xe4\x38\xca\xa7\x01\xbe\x4e", 7);
    *(uint32_t*)0x2000018c = 1;
    memcpy((void*)0x20000190, "\x46\xa7\x8b\xe3\x36\xa1\x14", 7);
    memcpy((void*)0x20000197, "\xae\xa1\x32\xde\x64\x05\xe2", 7);
    memcpy((void*)0x2000019e, "\x57\xf5\xcf\xf6\xd9\x30\xb7", 7);
    memcpy((void*)0x200001a5, "\x7b\xd9\x59\xd9\x58\x23\x77", 7);
    memcpy((void*)0x200001ac, "\x37\x78\x42\xf7\x5b\x81\x8e", 7);
    memcpy((void*)0x200001b3, "\x67\x3a\xe7\xfe\xed\xa6\x1d", 7);
    memcpy((void*)0x200001ba, "\xeb\x67\x17\x02\x52\x37\xb6", 7);
    memcpy((void*)0x200001c1, "\x1f\x39\xdd\xc1\x99\x70\x17", 7);
    *(uint32_t*)0x20000788 = 0x48;
    *(uint64_t*)0x20000790 = 0x20000500;
    *(uint64_t*)0x20000500 = 0x20000200;
    *(uint64_t*)0x20000508 = 0;
    *(uint64_t*)0x20000510 = 0x20000240;
    *(uint64_t*)0x20000518 = 0;
    *(uint64_t*)0x20000520 = 0x20000300;
    *(uint64_t*)0x20000528 = 0;
    *(uint64_t*)0x20000530 = 0x20000400;
    *(uint64_t*)0x20000538 = 0;
    *(uint64_t*)0x20000798 = 4;
    *(uint64_t*)0x200007a0 = 0x200009c0;
    memcpy(
        (void*)0x200009c0,
        "\x50\x00\x00\x00\x00\x00\x00\x00\x0c\x01\x00\x00\x00\x00\x00\x00\x84"
        "\xb0\xc5\xdb\x61\xeb\xe7\x09\xf7\xd7\xd6\x04\xa4\x71\x5e\x8e\x56\x13"
        "\x46\xd3\xcc\xce\xd7\xea\x85\x3d\xab\xa2\xe1\xc0\xcd\xc9\x25\x8d\x83"
        "\x56\x6e\x54\xa1\xdd\xbb\xf2\x8a\xd0\xde\x1f\x5c\x67\xca\xd5\xae\x29"
        "\x3b\xa4\x1a\xa1\xe3\x1c\xbe\x7a\x7b\xcb\x00\x00\x18\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x43\x24\x00\x00\x00\x00"
        "\x00\x00\xd0\x00\x00\x00\x00\x00\x00\x00\x19\x01\x00\x00\xa7\x00\x00"
        "\x00\x82\x33\x99\x8a\x04\x15\x3d\xab\x36\x52\x78\x85\x80\xf1\x8a\xa8"
        "\xd7\x63\x84\x54\x53\x70\x11\xea\x2c\xfc\x4f\xf5\xce\x80\xa9\x90\xf5"
        "\xdb\x68\xec\x38\x5c\x86\x51\x32\x0b\x8c\x34\x85\xf9\x07\x57\xf8\xb8"
        "\x07\xe9\xd7\x97\x3c\xfb\xc0\x91\x25\xfa\x86\x27\xc7\x2a\x6d\x97\xd8"
        "\xe0\x81\x55\x53\x94\xe3\x05\x29\x47\x0d\x93\x11\x14\xbc\x66\x79\x39"
        "\x54\xb9\xce\xb5\xd4\x48\x0d\xdb\xec\xe8\xc8\x34\x71\x20\xa6\x65\x70"
        "\x0e\xe9\x95\x1d\x81\xb1\x29\x03\x5f\x6c\x02\x88\x00\x64\xf0\xa5\x10"
        "\x58\x38\x82\x46\xd4\x72\x5f\x1b\x6a\x25\x07\x63\x07\x21\x51\xff\x2c"
        "\x31\x19\x29\xf3\xa3\xc9\xa2\x63\x1f\x66\x0d\xcb\xbe\x9a\x64\x93\x1f"
        "\xb2\x43\x39\xec\xa1\xa4\x82\x2b\xa1\xa5\x54\x70\x0b\xbb\xf9\x38\x01"
        "\x98\x17\x8c\x7d\xaa\x14\x82\x6b\x29\x62\x88\x85\xd1\x32\x00\xca\x25"
        "\x00\x00\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x00\x00\x00\x01\x01\x00"
        "\x00\x04\x00\x00\x00\xa9\x1b\x88\x2e\xca\xf0\xd0\x52\xe2\xf7\xe3\x0d"
        "\x85\x92\x2a\x86\x08\x3e\xf4\x85\x79\x09\x05\xc1\xc0\x4d\x0e\x99\xc7"
        "\xc9\x45\x48\x47\x90\x73\xa3\x4a\xdc\x23\x0a\x21\xb9\x6f\x33\xae\x7e"
        "\x38\x12\xc4\x78\xfb\x41\x0e\xbd\x19\x4f\x1b\x51\x3d\x09\x8c\xad\x13"
        "\xbd\x7c\x47\xd0\x66\xe8\x0e\xb7\xa9\x3e\x2d\xb7\xf7\xba\x0d\x03\xf5"
        "\xc5\x8a\x31\x0f\xaa\xd3\x44\x09\x2c\xd2\xf2\x1e\xaf\xae\x78\xb5\x36"
        "\x02\x49\x29\x1e\x95\x7f\x25\x82\xc7\xa8\x8b\x01\x6c\xf7\x72\xc8\x76"
        "\x5a\x82\xd0\xfd\xa7\x3e\x77\x75\xc3\xa0\x1d\x4e\x4f\x19\x29\x20\x10"
        "\x97\x8a\x87\x2d\x28\x56\x17\x79\x40\x5e\x6d\x53\xd7\x66\x0b\xf3\xe0"
        "\x59\x9c\x84\xf3\x47\x69\x4e\x52\x4d\xff\x36\x15\x88\x8b\x4c\x40\x86"
        "\x50\xea\x6f\xb6\xaf\x92\xf9\xd6\x39\x08\x49\xe8\x18\xe1\x94\xf5\x64"
        "\x03\xaa\x6c\x3b\x05\xe6\x74\x92\x65\x22\x2a\xfc\xc6\x27\xe3\x00\xa9"
        "\x06\x39\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x44\xa4\xe8\xe8\x2d\x34\xbe\x10\xc3\xd3\x32\xb8\xa7\x36\x70\x4e\x6f"
        "\xc9\x1e\x02\x6a\xb4\x24\xcc\x92\xe3\x3f\x5d\x32\xe9\x3e\xf2\x20\xb3"
        "\xdb\x6d\x58\xd3\xcb\x16\x18\x6a\x78\x95\x1d\xb5\x54\x1d\xd5\x0a\x5a"
        "\x6d\x46\xff\x51\x73\x8b\xc4\x89\xe4\xee\x7b\x02\x36\xd4\xf0\x6b\xa0"
        "\x0e\x2f\xba\x56\x48\x8c\x04\x50\x55\x70\x28\x8d\x1a\x07\x3d\xf2\xe6"
        "\xef\x58\x75\xd2\xc6\xa6\x87\x99\x56\xe4\x34\xd4\xfe\x9c\x55\x08\x1b"
        "\x77\x8b\xb1\xc6\x54\x50\x93\x3d\x63\xe9\xc3\xe2\x9f\x81\x10\x0e\xdd"
        "\x6a\x34\xc2\x92\x07\xde\xdc\x28\x5d\xf0\xab\xe3\xa0\xc6\x76\x93\xf1"
        "\xb3\xcb\x43\xde\xf0\xd8\x63\x59\x54\x7b\x4f\x9d\xf5\x04\x95\x32\x1c"
        "\x41\x1c\xee\x3d\x23\xcb\x3d\x01\xc5\x04\x62\x6e\x8d\x42\xef\x70\xda"
        "\x4c\xfd\x4e\xdb\x42\xa9\x37\xc6\x78\x9a\x8c\xeb\x49\xf2\x26\x34\xbf"
        "\xbc\xe0\x2b\x5b\x8c\x54\xe9\x5a\x08\xb3\x2d\x74\x3b\xe2\x64\x5f\x7f"
        "\x2c\xd1\x64\x05\x08\xd3\x67\xc3\x4a\x77\x62\x65\x4f\x3b",
        762);
    *(uint64_t*)0x200007a8 = 0x2fa;
    *(uint32_t*)0x200007b0 = 0x80;
    syscall(__NR_recvmsg, r[0], 0x20000780, 0);
    break;
  case 5:
    *(uint64_t*)0x20159fc8 = 0;
    *(uint32_t*)0x20159fd0 = 0;
    *(uint64_t*)0x20159fd8 = 0x20fc8000;
    *(uint64_t*)0x20159fe0 = 0;
    *(uint64_t*)0x20159fe8 = 0x20000e8e;
    *(uint64_t*)0x20159ff0 = 0;
    *(uint32_t*)0x20159ff8 = 0;
    syscall(__NR_sendmsg, r[0], 0x20159fc8, 0);
    break;
  }
}

void loop()
{
  execute(6);
  collide = 1;
  execute(6);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}