// DATA RACE in queue.(*Queue).Enqueue
// https://syzkaller.appspot.com/bug?id=8793cf6049f1261c498893450a9e80d3cbb46c9d
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

static void execute_one();
extern unsigned long long procid;

void loop()
{
  while (1) {
    execute_one();
  }
}

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

uint64_t r[3] = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};
void execute_call(int call)
{
  long res;
  switch (call) {
  case 0:
    memcpy((void*)0x20000040, "cpuacct.usage_percpu", 21);
    res = syscall(__NR_openat, 0xffffff9c, 0x20000040, 0, 0);
    if (res != -1)
      r[0] = res;
    break;
  case 1:
    memcpy((void*)0x20000080,
           "\x74\x65\x61\x6d\x5f\x73\x6c\x61\x76\x65\x5f\x30\x00\x00\x00\x00",
           16);
    syscall(__NR_ioctl, r[0], 0x89a1, 0x20000080);
    break;
  case 2:
    res = syscall(__NR_socketpair, 1, 5, 0, 0x20000200);
    if (res != -1) {
      r[1] = *(uint32_t*)0x20000200;
      r[2] = *(uint32_t*)0x20000204;
    }
    break;
  case 3:
    syscall(__NR_ioctl, r[1], 0x541b, 0x200000c0);
    break;
  case 4:
    memcpy(
        (void*)0x20000240,
        "\x04\xbb\xab\x05\x3d\xe5\x70\x6b\xac\xab\x8b\xba\xc4\x5a\x42\xc0\x72"
        "\x4a\x97\xea\x7c\x19\x24\x8b\xfb\xde\xa7\xb9\x2c\x32\x7a\xa1\x22\xf7"
        "\xd1\x23\x77\xaa\x73\x2e\x59\xdd\x6e\x22\x88\xdb\x32\x2d\x8f\x87\x0f"
        "\x9b\x43\xb5\x51\x0b\x06\x89\x91\x61\x15\x80\x66\xd2\xc8\x47\xdb\x92"
        "\xdf\x2b\x9a\xfb\x2b\x2e\x86\x13\x96\xb9\x29\x79\xf3\x8d\x3b\x99\x57"
        "\xc1\xf8\xe8\x42\x51\xde\x78\xb6\xce\xab\x85\x13\x2a\xf1\x04\xf0\xb1"
        "\x90\x59\x77\x11\x9c\xcf\x05\xc4\x7f\xc1\xf8\xee\x6c\xdc\xb0\x97\xee"
        "\x88\x3b\x70\x64\x01\x98\xa8\x97\xc4\x69\x3d\x2a\x41\x45\x96\x65\xc7"
        "\x0a\x41\xa2\x0c\x7c\x30\x25\x06\x24\x0b\x1a\x74\x2c\x29\x0c\xc3\x2d"
        "\x20\x39\xeb\x51\x62\xe6\xe5\xcd\xe7\xc3\x6f\x8d\xf2\xde\x89\xb4\xfb"
        "\x4f\xcc\xfe\x20\xf9\x76\xef\x88\x56\x46\x6b\xfd\xe8\x33\x35\xdd\xf1"
        "\x13\x66\xbb\xb1\x2d\x30\x41\xf8\xd1\x2f\x4b\xd1\xa4\xb2\x61\x21\x4a"
        "\xad\x9e\xd5\xe1\x58\x96\x5f\x20\x88\x2e\x49\x7b\xc4\x65\x42\x9b\x55"
        "\xad\xfc\xac\x7e\x40\x5f\x7f\xe9\x85\x2f\xce\xaa\x98\x08\x21\xa9\x5c"
        "\x00\xd6\x97\x4d\x79\x8a\xb7\x0d\xdb\x87\x12\x30\x65\xcf\xd9\x8a\xde"
        "\x31\x4b\xdc\xd1\x7b\x48\xf8\x77\x72\x07\xff\x14\xbc\xe7\x5a\xd4\xcc"
        "\xc8\xc4\x58\x5c\x88\x9b\xd1\xc0\x86\x68\x96\xc7\x09\xa8\x1a\xc7\xe5"
        "\xfd\x15\x17\xd6\x14\xbf\x9a\x5f\x9c\x82\xc8\x91\x25\x68\x34\x8e\xfd"
        "\x69\x9c\x10\x24\x1f\x8e\x56\xb7\x72\xd1\x8a\x81\x39\xbb\xe3\x73\xd4"
        "\x97\x35\x27\x97\x28\x20\x6c\xa0\x23\xb4\x60\xae\x8e\xc6\x62\x83\xc7"
        "\x61\xcd\x1f\xfa\x84\xcd\xd8\x18\x17\x0b\xb8\x2f\x87\xa7\x08\xe5\xb4"
        "\x79\xb4\xf3\x6a\x87\xdd\xa7\x7c\x32\xb1\xa0\x84\xc2\x17\x7d\xa5\x45"
        "\x7a\x7e\x68\xfa\xd5\x72\x5c\x62\xff\x8a\x19\x97\x18\x30\xc0\x5a\x47"
        "\xcc\x38\x9a\x9a\xa6\xb6\xe0\xb7\xdb\xca\x08\xcf\x70\xaf\x02\x87\xa0"
        "\x30\x51\xf6\x4e\xa8\xa2\x56\x15\x3e\xc5\xf3\x75\xa3\x20\x19\x49\x25"
        "\x42\x96\x0d\xe8\x39\x94\x9b\xa9\xb8\xf2\x1d\x31\x49\xb0\xa6\x28\x99"
        "\xf2\xb5\x66\xd9\xe5\xcc\x0b\x2f\xc9\x41\xda\x2f\x64\xe8\x6e\x39\xad"
        "\x5a\xc4\x11\xf1\xaf\x07\x34\x27\x85\x27\x2f\x52\x6f\xe7\xc7\x3d\x5b"
        "\x6c\x62\xae\x33\x25\x14\xb9\xf4\xd0\x2d\x55\x10\x1a\x38\x34\x0d\xf5"
        "\x96\x0e\xf2\x82\x70\x52\xa1\x67\xf6\xfe\xb8\xa7\xaf\xd9\xe4\x69\x2e"
        "\x67\x03\x8e\x58\xfd\x85\xd5\x2f\xe5\xe2\x75\x26\x19\x83\x71\x74\x14"
        "\xe5\x60\xd0\xb1\x39\xc5\x7d\x09\xf3\xe4\xfd\xd3\x4d\x5e\x6c\x42\x4d"
        "\xf9\xaf\x22\x77\xcd\x80\x5f\xff\x70\x4d\x4b\x0c\x48\x43\x09\x2b\x77"
        "\xa4\x74\x44\xfd\x92\xba\xee\xbd\xb4\x94\x3c\xf0\x22\xd9\xf3\xc1\x9d"
        "\x5d\xe7\xaa\x4a\xd5\x76\x55\xba\x56\x90\x1b\xa6\x76\xeb\x83\x5e\xda"
        "\xb5\x43\x0f\xfd\x02\x51\x07\xdb\xab\xec\x18\x2e\xb3\x4a\x72\x67\xc0"
        "\xac\xd1\x39\xcb\xa5\x10\x88\x9c\x72\x59\xcb\x2b\x79\x1e\x91\x85\xff"
        "\xd9\x05\xaf\xd2\x12\x40\x09\xd9\xf6\xd4\xd4\x97\x55\xe6\x8f\x27\x19"
        "\x73\xd3\x5b\xd7\x1c\x5d\x4e\x8f\x8f\x8e\x11\x29\x33\x5c\xe6\x12\x65"
        "\x4e\x55\xca\xfc\xaf\x27\x1d\xb3\x80\x22\xa6\x9b\xaf\x9b\x4a\xfd\xb3"
        "\xce\xb7\x65\x8c\x07\xd2\x1b\x4a\xa3\x79\x5f\x4b\x36\x9c\x09\x5c\x9b"
        "\x04\xba\x4d\xdc\x93\xbe\xa8\xc5\x9a\xbc\xe3\xce\xc1\xa2\x28\xb7\xc5"
        "\x69\xf8\x79\xe5\x9b\xde\xbb\x78\x0c\x79\xd8\xc8\xb7\xa0\x59\xd3\x3a"
        "\x46\x02\x5b\xd5\x5b\xed\x32\x76\x6c\x1c\x8c\xf6\x5d\xb1\x89\xd4\xb9"
        "\x8b\x91\x68\xd3\x3f\x40\x89\xe6\x1d\xdd\xf0\x82\x30\x53\x24\x86\x6f"
        "\xdb\x7b\xdb\xd9\x45\xdf\xe0\x94\x45\x7f\xb2\xb4\xf6\x7c\xd7\x05\x63"
        "\xa5\x57\x8c\xe5\x9f\x69\x82\xbe\x62\x1a\xbf\x66\x39\x80\x3f\x22\xa1"
        "\x45\x26\x58\xa2\xe1\x85\x96\x21\xbb\x55\x06\x47\x81\xec\x34\x4e\x9a"
        "\xe4\x59\xcf\xab\xa6\x16\xfc\x24\x64\xb2\xe9\xa0\xb9\x44\xf0\x88\x6b"
        "\xd6\x23\x5d\xca\x2d\x60\x65\x87\xc6\xd6\x3d\x98\xf1\xf9\x71\x41\x3a"
        "\x27\x14\xf4\x86\xd1\xe7\xa6\xba\x89\x30\x1b\xfa\xd8\xc1\xc8\x54\xda"
        "\x23\xad\xba\x81\x05\x02\x65\xc4\xc2\x05\xc1\xe5\xf6\x77\xa2\xcf\xef"
        "\x5c\x7e\x92\xc0\x33\x3b\xd7\x91\x45\x8d\x20\xb1\x28\x8e\x36\x68\x99"
        "\xbe\x74\x64\x4b\x54\xdb\xd2\x2f\x42\x2b\x62\xd1\x01\xf1\xf6\xb3\x47"
        "\xa7\xca\x8a\xce\xa8\xa6\xde\x87\x4a\x73\x02\xe5\xc1\x68\x84\x7e\x3a"
        "\x86\xba\xf5\xed\xb0\x10\xd5\xab\x7a\xe2\xa5\x6d\xfd\x7c\xba\x45\x82"
        "\x80\xa6\xcf\x7d\x97\xc0\x01\xa2\xf2\xac\x5f\x14\xc4\xea\x8f\x74\xe3"
        "\x79\xac\x36\x7f\x8d\x34\x43\x6b\xdc\x18\x62\x28\x48\x40\x0b\x23\x8c"
        "\x37\xc5\x5a\x78\x70\xb5\x75\xd4\xa9\xcd\xd4\x20\x94\xf0\x3b\xf3\x7a"
        "\xf1\x27\xba\x36\xa0\x64\xbc\xa3\x25\x6f\xa4\x2c\x27\xaf\xa7\xd7\x7d"
        "\x5b\xe2\xa7\xa7\x98\x33\xb5\x2c\x07\x37\x27\x16\xc5\xc7\x61\x0c\xbf"
        "\x03\x76\x24\x93\xcc\x63\x11\x29\x45\xec\xa7\x4c\xa7\xac\xe2\xb1\xea"
        "\x30\xb0\x87\x55\x26\x79\x98\xe7\xa7\xd1\x54\x6d\xc0\xfe\x91\x31\xd2"
        "\xa9\x79\x4d\xde\xc9\x10\xd7\x06\x08\x82\x92\x61\xb4\xe3\x9a\x68\x73"
        "\xaf\x9e\x4d\xa0\xd8\xfb\x27\x17\x5c\x98\xc0\xde\x36\xc7\x36\x73\xf8"
        "\xbe\x78\x8a\x54\xc8\xe5\x07\x15\x61\xaa\x3f\x6c\x00\xf5\x87\xc9\x3e"
        "\xc0\xfd\x43\xda\x68\x46\x0e\xbf\x3d\xfa\xa4\x2f\xdf\x38\x92\x7a\x8e"
        "\x9f\xa1\xb7\xae\x66\x03\xd6\xb3\x23\x2e\x48\x0f\xde\xa5\x99\x1a\x9c"
        "\x70\xbc\x7e\x03\x0d\x7f\x47\x1f\xd5\x4f\xb8\x97\x55\x9c\xd3\x33\xfa"
        "\x54\xad\x32\x19\xdf\x90\xcb\xf5\x08\x35\xec\x00\x2e\xe8\x9c\x41\x88"
        "\xbd\xfd\x84\xe1\x5e\xc4\x4d\x55\x73\xcd\xd2\xdc\x9a\x04\x78\x82\x80"
        "\x8b\x8e\x95\xa1\xda\xc9\x3f\x88\xf6\xec\x4a\x66\x1a\xce\xb6\x86\xea"
        "\x92\x08\x43\x41\xd6\x63\xeb\x7b\xc1\x83\x31\x5e\xed\xca\x9a\x61\x86"
        "\x23\x83\x85\x2d\x54\x61\xcc\x98\x51\x6b\x3f\x7b\x9b\x1c\x9c\xe2\x3f"
        "\xea\x85\x54\xea\x07\x81\xde\xa8\x31\x90\xdf\x39\x6e\xea\xba\x38\xa3"
        "\xa1\x0e\xdb\x3b\x59\x46\xff\x6b\x3e\x70\x6d\x57\xfb\xde\x15\x33\xdf"
        "\x4f\x76\x2d\x4a\x21\xf8\x7e\xec\xc9\x10\x93\xc2\x1b\x7f\x14\x3e\xa0"
        "\x7e\xac\x3c\xbf\x50\xaf\x31\xd3\xcc\x6e\xa4\x6d\xb5\x9f\x25\xc8\x4c"
        "\xc9\xeb\x29\x93\xcf\xff\x42\x50\x88\x00\xa4\x8f\xb4\xc2\x5a\x67\x8c"
        "\xbf\xdb\xef\xba\xfb\x56\x07\x35\xb7\x91\x57\x4c\xed\x45\x4c\x6e\xa7"
        "\xb4\x38\x84\x6b\x7e\xc9\x7e\x57\x6b\x81\x1f\x2f\xba\xb2\xa2\x10\xb8"
        "\xd8\x9e\x0d\xd0\xd3\x7b\x99\xfe\x91\x38\xaf\xa2\x11\x57\x81\x6d\x69"
        "\xa8\x42\xbb\xa6\xf4\x28\xac\x48\x40\x7d\x1d\x46\x74\x06\x91\x34\x5b"
        "\x13\x36\xc2\x72\x03\x6c\x4b\x57\x9f\x57\x7c\xe3\xf3\x38\x75\x44\xc0"
        "\xb3\x6d\xe0\xe2\xb6\xf7\x1f\x00\x63\xc5\xd7\x24\x6e\x78\xfa\x9e\x60"
        "\x5a\xb9\x34\xf8\xef\x1d\x29\xbd\xee\x79\x76\x27\x27\x43\xdd\x23\x12"
        "\x6f\x64\xb7\x0e\x11\x79\xbf\x25\x16\x3b\x22\xad\x00\x48\xbf\xf9\x88"
        "\x83\xf1\x52\x58\xa7\x9c\xae\x35\xff\x99\xbc\x3a\xc3\x99\x61\x76\xb9"
        "\x25\x69\x9a\x89\xcf\xf7\x14\x14\x0a\xf7\xeb\xcb\x1c\x62\x2f\xe5\xb6"
        "\x61\xf6\xb2\xc8\x58\xb6\x32\xcd\x8a\x48\x1e\x87\x23\x66\xa6\x6a\x8b"
        "\xe6\xc9\x0d\x9f\x5d\x09\x67\x9f\x11\xb5\x0e\x19\xbf\x28\xe0\xa8\xe9"
        "\xb3\x77\xba\x55\x1d\xff\xb4\xfc\xe8\xf9\xb5\xf0\xd8\xa1\xae\x92\xcb"
        "\xa0\x51\x19\x1f\xf6\x80\x0c\x43\xd7\x59\x85\x76\x3b\xc7\xb6\x94\xcb"
        "\xd4\xc1\x17\x18\xd7\xb2\xd8\x7a\x13\xd7\x91\x31\xa4\x7b\xf9\x9e\x38"
        "\x01\x3f\xbf\xd2\x60\xa3\x1f\xd6\x40\xc3\xf4\x1e\x80\xdc\x7d\x90\x07"
        "\xf0\x7c\x7f\x15\x2a\x9f\x82\x2e\xe4\x5e\x69\xd4\xa7\xd0\xec\xf4\x07"
        "\xda\x69\x65\x8b\xa2\xa5\xe0\x44\x1f\xcd\x8e\xde\x32\x31\xd6\x36\xdd"
        "\x1c\xea\xc7\x53\xbc\x7b\xc7\x41\xaa\x73\xe9\x9a\xd2\xc2\xde\xd7\x94"
        "\xe2\x40\x2a\x63\x77\x48\x95\x6d\x46\xd1\xae\xef\xef\xed\x54\x70\xfd"
        "\x6b\xeb\x3c\xcb\xf5\x96\xce\xff\x3b\x3a\xbf\xf0\x92\x91\x85\x15\x1d"
        "\xbc\xda\x84\x3d\x7b\x76\xd6\x55\xc7\x9a\x38\xd0\xe2\xe7\x03\x4e\x67"
        "\x46\x61\xcf\x96\xa3\x2f\x89\x29\xb2\x76\x43\xb5\xe7\x25\x87\xb7\x8b"
        "\x59\x7d\x24\xf5\x08\xb6\xa8\x9f\x5e\xa8\xb6\xb0\x4c\xa1\xff\x6e\x60"
        "\x8b\x59\xa4\x75\x86\xf0\xab\x8c\xdb\x99\xb3\x59\xaa\x7f\x17\x64\x84"
        "\x38\x6c\x89\x6f\xd8\x88\x8f\x6a\xb0\xde\x2a\xe5\x1f\xca\x94\x49\xc4"
        "\x47\x2d\xeb\xba\xfd\x45\xb7\x61\xb5\x26\x89\xa6\x45\x20\xea\x87\xc8"
        "\x50\x33\x1e\x80\x6c\x30\xe6\x02\xa4\x89\x38\x8f\xa5\x4e\xbe\x82\x82"
        "\xb2\xf2\xff\xb0\x42\x68\x66\xce\xcb\x9f\x7f\x9f\x83\xe0\xe5\xef\x69"
        "\x2e\x8c\xcc\xda\xa2\x28\xdf\xf9\x0e\x71\xbb\x0c\x9a\xbf\x4c\xe4\xe8"
        "\x2d\xa0\x5a\x69\x83\x17\x2e\x74\x97\xb0\xba\xd0\x1b\x2a\xd0\xe3\xa5"
        "\xba\xe0\xe8\xf0\x62\x3f\xdd\x8e\xc1\xfb\xfc\x97\xc0\x8e\xb7\xb8\xea"
        "\x4e\xf0\xe8\xbd\x85\x98\xf7\x7d\x0d\xf9\x92\x48\x24\x1e\xfd\x37\x15"
        "\xff\x59\x56\x2e\x9c\x36\x68\xfb\xc3\x33\x0b\x2e\x5d\xdf\x6d\x96\x87"
        "\x8d\x4e\xbd\x94\xc1\x60\xe8\x7d\xe0\xd0\x3b\x36\x64\xbe\xeb\x71\xa7"
        "\x93\x24\x90\xb2\x64\x1b\xd8\x49\x30\xa0\x38\x3b\xae\xcb\xdc\x0f\x6b"
        "\x53\x72\x20\xf3\x5c\x4c\xe2\xbc\x45\x87\xa8\x66\xa4\xcb\x15\xf0\x93"
        "\xe6\x3b\x32\xef\xbe\x9a\xa5\x91\x07\x6a\xaa\xf1\xa8\x80\xf5\xd4\x21"
        "\xe2\xe0\x5f\x60\xce\x53\xd1\x9b\x80\x34\xe3\xac\x5d\x2a\xe0\x2d\xac"
        "\xb3\x06\xc5\x64\x72\x26\xfc\x1d\xa6\xf0\xdc\x11\xc4\x83\x64\x72\x71"
        "\xcd\xd6\xb0\x44\xcc\x03\xd3\x9e\xc4\x51\xde\x74\x0d\x18\x5f\xf9\x1b"
        "\x7b\x58\x6d\xec\x35\xb0\xac\xfe\xe2\xab\x7c\x8b\x40\xd9\xff\x8d\x39"
        "\xe2\x71\xea\xd9\x34\x74\x6c\xde\xa4\x93\x49\x9c\x16\xa9\xd7\xf8\x99"
        "\x42\x0f\x59\x34\xbc\x8f\xdd\xe6\x4b\x63\x83\xc0\x12\x97\xa0\x16\x29"
        "\x1e\x60\x28\xff\x49\xff\xa2\x29\x67\x2c\x15\x39\xc5\xed\x7b\x6e\xbf"
        "\x61\x43\xa0\xd7\xd1\x8e\xf3\xfc\x5c\x3d\x56\x71\x6d\x90\x71\x14\x3d"
        "\x9f\x60\x6e\xe4\xb2\xa2\x9f\x51\xda\x3d\x1a\x45\xb6\xe4\x37\x6f\x41"
        "\xaf\x21\x80\xf9\xa6\x70\x59\x9d\xb7\xa1\xa3\xe5\x4b\x37\x48\x0d\xd6"
        "\xe9\x02\xec\xd1\xb6\x3b\x88\xcf\x1f\xbb\x8c\x5d\xe8\x76\x37\x1c\x2d"
        "\x96\x0b\x9c\x2d\x88\x21\xad\xa4\xc2\x7a\xc7\xd7\x03\x1f\xf7\x86\x6d"
        "\x39\x65\xc8\xd1\x6b\x52\x92\x8d\xf8\xda\x6e\x74\x38\xa3\x7d\xbd\xd1"
        "\x4a\xb3\xfb\xd1\x76\x58\x3a\x58\x26\xad\x31\x40\x2b\x2b\x4a\xf1\x39"
        "\x0b\xd6\xa1\x05\x52\xc5\x21\x89\x8c\x32\x7c\xb0\x41\x0e\x30\xa1\x93"
        "\x39\xc6\x7f\x51\xbc\x4c\x79\xd8\xc2\xde\xa1\x87\x0d\x92\x48\x48\x90"
        "\x9d\xaa\x21\x0a\xd4\xb8\xee\xa4\x5e\x5e\x0b\x77\x8f\x9e\x55\x44\xa4"
        "\xf5\x49\x73\x0c\x35\x21\x15\xec\x0a\xfa\x60\xda\x7c\xb0\xbf\x2f\x67"
        "\x07\x9c\xd6\xeb\x7f\x8c\xf4\x66\x30\x4a\x66\xfb\x31\xe8\x9c\x9d\x74"
        "\x91\x51\xad\x35\x86\x6e\x80\x8f\xe6\xe6\xf9\xe3\xd1\xa2\xa8\x95\x30"
        "\x68\x6c\x9a\x9c\x4f\x78\x2a\x54\x03\x2b\x4a\xd5\xf5\x94\xcd\xcd\x01"
        "\x50\xea\xae\xb8\xdd\x72\x40\x2a\x88\x4a\xd1\xe3\x4c\x6b\x3c\x16\x9d"
        "\x1a\xca\xed\x04\x99\xfd\x58\x4d\xa9\x7e\x91\x9a\x72\x32\x76\xea\x47"
        "\x47\xf0\xad\x51\xb5\xfc\x74\x13\xc6\xf0\x87\x1b\xb9\x9b\x7b\x2c\x12"
        "\x82\x56\xc1\x23\x0b\x3b\x48\xe4\x71\xc4\xc5\xe3\x95\xea\x02\xb8\xdc"
        "\x71\x95\x24\x24\x83\xde\x00\xff\xeb\x75\x23\x26\x98\x3e\x37\xf8\x4f"
        "\x3e\x4f\xd1\xb8\x70\x36\xdb\x0b\xb0\x05\xab\xf5\x8c\x74\x43\x12\xbf"
        "\x55\x75\x05\xaa\x9e\x43\x2d\x47\x63\x0f\x5e\xb0\x04\xa4\x79\x61\x0a"
        "\x85\x67\xd9\x60\x81\x26\x23\xad\xb7\xc8\xbd\x14\x2e\x18\x8b\x6d\x20"
        "\x10\x28\x41\xbc\xcc\x72\xf0\xfb\xe6\xdd\x65\x1b\x8e\x1c\x13\x08\x77"
        "\xd1\x02\x54\xaf\xfe\xe4\x0e\x45\xa8\x8a\x69\x91\xaf\x9b\x64\x5f\x3d"
        "\xca\xa1\xc1\x43\x85\xc7\x7b\xd6\xb8\x30\x75\x28\x15\x5b\x50\xdc\x0a"
        "\xf1\x85\x27\x02\xda\x10\xee\x52\x62\x49\xd0\xb2\x0b\xb3\x2e\x08\xa8"
        "\xc0\xcd\x93\xa3\xac\x6e\x5c\xc4\xb7\xe7\x0a\xcc\x2e\xef\xd0\xb3\x8c"
        "\xb3\xea\x58\x1b\x73\x27\xf3\x99\x31\x46\x3b\xfa\xe2\x6c\x1a\x8b\x5b"
        "\x0f\x37\xe2\xb5\xa0\x31\x5f\x29\x0f\xdb\xba\x1b\xc5\xb2\x02\x09\x63"
        "\x84\x3a\x07\x1b\xd1\x33\x24\x87\x7e\xe4\x58\xf7\x89\xfc\x5a\x32\x3f"
        "\x27\x4d\x07\x54\xa8\xba\x83\x04\xad\xec\xbd\x43\xb1\x22\x1c\x81\xe3"
        "\xa1\xed\x69\xb2\x55\x77\x71\x65\x65\x09\x78\xad\x16\xca\xa0\x62\x20"
        "\xf7\xf3\xee\x90\xfc\x31\x87\xb4\xed\xc5\x7d\xc0\xfd\x2b\x85\x55\x60"
        "\x87\xef\x35\x65\xb7\x55\xd8\x5f\x8e\x49\xbd\x5f\x25\xa5\x43\x37\x74"
        "\x6b\x4f\x7c\x6f\x88\x06\x9f\x12\x96\x8c\x8c\x4d\x45\xf8\x67\x37\x42"
        "\x4e\xe1\x79\x38\xe5\x86\x6f\xa0\xf8\x2b\x73\x6a\x56\xcf\x14\x1a\xe3"
        "\x4e\x49\x71\x61\xe8\xf5\xb5\x9b\xd8\xd4\x24\x9c\x4f\x65\x12\x5e\x13"
        "\xac\x24\xb9\xb4\x9d\x9c\x6c\xda\x92\xee\x38\xff\xc3\x50\x98\x84\x14"
        "\x91\x6a\xc6\xf8\x5e\x70\x58\x3c\x32\x2e\xea\x3b\x05\x91\x09\xe4\xbe"
        "\xb3\x5a\xd1\x73\x6b\x8b\x42\xf6\xbc\xe0\xa6\x96\x28\xbb\x37\x91\xe7"
        "\x5f\x77\x45\x84\xbe\x4d\x9e\x7f\xf5\xaf\xb7\xe3\x62\x7a\x24\x22\x48"
        "\x5d\x83\x12\x20\xdc\x48\x84\x27\xf5\xe0\x07\xa1\xfd\xbe\xae\x00\x7e"
        "\x01\xfb\x25\xb9\x51\xdd\xaf\x88\xef\x02\xd9\x43\xd7\x04\x91\xd6\x05"
        "\x51\xd5\x73\xe5\x4d\xef\xd8\x35\xa7\x62\xe7\xcf\x34\xa9\xec\x6c\x48"
        "\xa8\x88\x10\xa5\x94\x41\xb9\x6d\xbc\x58\x24\x4b\xb8\x70\xd4\x0c\xdd"
        "\x1c\x72\xde\x66\x7a\xdc\xaa\xb3\x25\xd3\x28\xfd\xec\xd2\x6f\x66\x67"
        "\x61\x3d\x39\x12\x4f\x09\x95\x00\xb5\x6c\x1b\x24\x4b\x4d\xf8\x08\x00"
        "\x6f\x95\x53\x99\xfd\xbc\x73\xa3\x4e\x02\xa6\xec\x3f\x68\x78\xd5\xe9"
        "\x8e\x77\x83\xd4\x76\xf5\x15\x59\x57\x95\x5a\xae\x28\x69\xc5\x22\xa4"
        "\xd8\xb1\x43\x39\x91\x01\x2b\xcd\xf3\x5c\x16\xbf\xb0\x10\x53\xfb\x15"
        "\xe7\xab\x79\x04\xba\x5a\xea\xeb\x7e\xd3\xa3\x2a\x67\x0f\xe6\x6b\xd0"
        "\x3f\xa7\x2e\x23\xb7\x77\x20\xe2\xd3\x6e\x16\xbb\xb2\x47\x7d\x65\xcf"
        "\xdf\xc2\x5c\x38\x52\x6f\x10\x6e\x37\x54\x00\x37\x1e\xa6\x26\x36\x69"
        "\x4f\xf5\x09\x2a\x79\x63\x7f\x01\xc4\x94\xa7\x14\x71\x59\x74\xbd\x08"
        "\xf2\x91\x69\xb2\x6e\x82\x97\xf0\x7b\x6a\x27\x3a\x7a\x55\x62\xc1\x54"
        "\xf5\x8e\x65\x82\x79\x16\x80\xcf\x6b\xff\xf2\x65\xec\x9e\xc1\xe2\x30"
        "\x2a\x8c\xbf\xa9\x93\x1d\xfd\xa3\x23\x6f\x10\xc1\x68\xca\x54\x84\xa4"
        "\x6e\x80\x31\xca\xe9\x9a\x5d\xb2\x3f\x81\x4e\x28\xb2\x26\x00\x6b\xcc"
        "\xf1\xf1\xf6\x0b\x96\xb7\xf9\x3d\xa5\x02\x0a\x51\x15\xd9\x58\x40\xd5"
        "\xea\x60\x1b\x1c\x6f\xb6\x81\x87\xd0\x0c\xc2\x24\x99\xd5\x6a\x30\xf1"
        "\x10\x24\x5e\xc8\xd6\x09\x6a\xce\xc9\x0c\x35\x2e\x3c\x3d\x97\xd6\xe6"
        "\xed\x0b\xc5\x41\xaf\x4a\x8f\xb5\x67\x54\xdb\xd1\xbe\xb9\x66\x13\x12"
        "\xd6\xbe\x01\xf1\xf9\x59\xd5\xc3\xf0\x86\x20\xa7\xd7\x35\xa6\x13\x7c"
        "\x28\x63\x2e\x85\xdf\x38\xb6\x56\x10\x68\x8e\x02\xfc\x76\x4b\x2d\x76"
        "\x1b\xee\xa0\x93\xb0\x30\x10\xdb\x0b\x59\x9f\xbe\x32\x63\x71\xe1\xf4"
        "\xd7\xfd\x39\xc2\x1d\xea\xf8\xb4\xca\x4d\x8e\x56\xe7\xfd\x18\xa8\xa4"
        "\xd0\x6b\xb0\x26\xf0\x77\x9e\x9c\x0d\x9a\x54\x86\xcb\x55\x78\x99\xc0"
        "\x48\xb1\x0d\x94\x2c\x2f\x24\x36\xac\x73\xd1\x36\xb2\xa6\x36\x0d\x61"
        "\x01\x67\x77\x49\xfd\xcf\x30\x0e\x99\xbc\xa9\x61\xbe\x0e\xdb\x8c\xed"
        "\xbf\x27\x54\x97\x74\x36\xc3\xa5\xa7\x08\xcc\x1b\x7f\xc0\xd5\xe4\x46"
        "\x0f\x91\xa4\x00\x3e\xfb\xd0\xdd\xe9\xe2\xa2\x69\x89\x5b\xce\x9b\xa6"
        "\xdb\x65\x2f\x5e\xad\xec\x81\xd7\x9f\x7f\x19\xd6\xb3\x35\x10\x23\xca"
        "\x25\xe0\x28\x98\x7d\xb8\xb9\x4d\x08\x35\xcd\x23\x6d\x8c\x83\x08\x22"
        "\x40\xda\x5b\xc3\xb9\x21\x99\x21\x03\xd8\x06\x62\xb9\x1c\xec\xf9\xca"
        "\x3d\x49\x2e\x29\x3a\x81\xf3\xfe\xcf\xae\xe7\xc9\x04\x6f\xab\xcc\xab"
        "\x60\x2d\x56\xd7\xfc\x16\x4d\xaa\x43\x08\x9d\xa9\x83\x11\x1b\xef\xee"
        "\x3f\xae\xa3\x54\x77\x8d\xec\xcc\x49\xee\x2f\x7d\x8b\xc0\x50\xb8\xb2"
        "\x2f\x71\x32\xbb\x21\xb7\xeb\x8c\x74\x16\x54\x5a\xd2\xb9\xde\x1f\x65"
        "\xf0\x85\x47\xf0\xf3\xd3\x9d\xac\xc7\x81\x5c\x25\xf9\xad\xa9\x8c\x61"
        "\x6b\x00\x98\xd6\xe7\xc6\x08\xea\xff\xfd\xd5\xd6\xab\x84\x86\xf0\xc2"
        "\xb8\xd4\x5b\x5c\xeb\xd4\x7b\xc8\x4b\xdf\x65\x7c\xda\xa5\x42\xd7\x5b"
        "\xd0\x82\xb1\x5e\x96\x46\xe7\xe5\x87\x39\xb4\xf6\xb8\x24\x37\x2a\xf8"
        "\xad\x65\x6b\x65\x93\x0c\x9b\xe9\xe9\x06\xa3\x54\xf6\x61\xa8\x12\x96"
        "\x7c\xbc\x55\x5b\x97\xd3\x1c\x63\x9f\x24\x49\xbb\xc5\x29\x29\x0b\x83"
        "\x57\x17\xc1\xce\xdf\x58\x7d\x58\x77\x5f\xdf\x4d\x3a\x70\x52\x14\x1e"
        "\xef\xea\xf5\x48\xf3\x8b\xf6\x73\xb5\xfb\x04\xe2\x02\x2b\x3a\xda\x9e"
        "\xa0\xfa\x7c\x22\xd3\xe2\x25\x38\xec\xcf\x4d\x72\xa9\x31\x8a\xf2\x52"
        "\x1e\xfb\x47\x02\xcd\xc4\xa7\x34\x1e\x1c\xe5\xe8\xb6\x76\x92\x48\x75"
        "\x3a\x36\x4f\x5f\xec\x7e\xef\xd8\xf4\x2f\x6b\x37\x72\x63\x95\xae\x81"
        "\x86\xc0\xdc\x1f\x58\x60\x35\x0b\x7b\x78\x96\x0e\x54\x4a\x86\x9e\x68"
        "\x4c\xd5\x3a\xa9\x93\x61\xbc\x6d\x53\x32\x6f\x3b\xbe\x93\xd5\x2d\x45"
        "\xc2\xe0\xfc\xc1\xd6\xef\x7b\x39\x48\x04\xea\xce\xd4\x10\xd1\xc4\x83"
        "\x0b\x3c\x52\xc7\xc3\x88\x4f\x54\x96\x9d\x73\xd3\xec\x0c\xf2\x1a\xa1"
        "\x4c\x8d\x24\x72\x3c\x6f\xdf\x9e\xb3\x74\x81\x21\x4b\xc4\xb0\xea\xb6"
        "\x1c\xe9\xc3\xec\xf3\xfd\x24\xb3\x86\x20\xc1\x31\x10\x59\xc8\xc5\xfb"
        "\x17\xed\x87\x68\xe2\xa9\xeb\xa2\xc0\x6a\x2d\xc7\xe6\xcf\xe3\x4f\xc7"
        "\xcc\x22\xa3\x25\x69\x20\x03\xb8\x06\xe3\xb6\x4b\x6a\xe6\x6a\x83\x87"
        "\xa5\x60\x38\x18\xb3\x71\x17\xa6\x9e\xed\x4d\xc6\xcc\x76\x25\x28\x24"
        "\x51\x82\x28\x5c\xeb\xb4\xb3\x73\x1f\x90\x1a\x1f\x7c\x59\x90\x2e\xf1"
        "\xcf\x33\x12\x65\xc4\x25\xd6\x18\xda\xea\x52\xbf\xd6\xb3\xc9\xbc\xbb"
        "\xb4\x67\xf2\xcc\x28\x7f\x16\xaf\xbc\xe9\x40\xd3\x25\x37\xa9\x14\x32"
        "\x08\xe8\x9f\xbf\x96\x81\xe1\xdd\x70\xcd\xa3\xa9\xb2\x6d\xdb\x61\xcb"
        "\x16\x3e\x96\x8a\xbd\x27\x68\xf1\x8d\x65\x69\x0d\xbb\x5a\x3e\x1f\x1f"
        "\x81\x42\x32\xf3\x76\x9e\xfb\x5a\xda\x54\x2a\x47\xea\x1d\xee\xdc\x46"
        "\x44\x6a\x44\xe9\x1c\x95\xe0\xae\xe6\xd0\xbc\x59\xf6\x9a\xb8\x2d\x22"
        "\xb8\x49\x65\x83\x28\x25\x7b\xb5\xcc\x51\x5c\xdc\x1d\xe6\xee\x86\xdd"
        "\x63\x1c\xb2\x23\x8b\x35\xcb\x45\xc9\x22\xcd\x16\x17\x23\xc2\x34\xdb"
        "\x32\x44\xb7\x9b\x41\xef\xc5\x2f\x6f\x58\x28\xb5\xf4\xb4\x39\x69\x42"
        "\x3b\xc5\x8a\x5b\xfc\x90\xac\xbd\xe1\x1a\xa6\x95\x13\xc6\x68\xb6\xd3"
        "\xc2\xe6\xae\x27\x4a\x42\xd1\x99\xbb\x9f\x6e\x4b\x5f\x95\xf2\xd9\x25"
        "\x38\x0f\xe7\x92\x9a\xda\xc6\x29\x22\x31\xec\x6d\x91\x3e\xb0\x9f\xc2"
        "\x12\x65\x97\xcb\x70\x80\x53\x6f\xb3\xdb\x59\x1b\x06\x95\x73\x37\xc7"
        "\x55\xb8\xef\xec\x13\x98\x9e\x8f\x79\x56\x9b\x68\x7b\xdc\x18\x25\xa5"
        "\x3f\x91\xec\xac\x2e\x07\xcc\xe9\x5f\x7d\x0d\xb2\xb6\x51\xbd\x16\x1b"
        "\xf8\xee\x2f\xf0\xdd\x47\xd9\xad\x20\xc8\x1c\x7b\x8b\x7c\xf2\x66\x15"
        "\xb8\xd7\x1e\x11\x62\x9a\xfc\x81\x9c\x61\xdb\x15\x77\xab\x0f\x26\xa8"
        "\x90\x2f\x30\x3e\xac\x8d\xfb\xf1\x55\xfe\xc0\x9c\xa4\xa7\xe8\x08",
        4096);
    syscall(__NR_sendto, r[2], 0x20000240, 0x1000, 0x20000000, 0, 0);
    break;
  }
}

void execute_one()
{
  execute(5);
  collide = 1;
  execute(5);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  for (;;) {
    loop();
  }
}
