// KASAN: global-out-of-bounds Write in string
// https://syzkaller.appspot.com/bug?id=60a6a7457bf43c7f3c52b52ff55231e06ab859ac
// status:fixed
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <fcntl.h>
#include <linux/futex.h>
#include <linux/loop.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

static uintptr_t syz_open_procfs(uintptr_t a0, uintptr_t a1)
{

  char buf[128];
  memset(buf, 0, sizeof(buf));
  if (a0 == 0) {
    snprintf(buf, sizeof(buf), "/proc/self/%s", (char*)a1);
  } else if (a0 == (uintptr_t)-1) {
    snprintf(buf, sizeof(buf), "/proc/thread-self/%s", (char*)a1);
  } else {
    snprintf(buf, sizeof(buf), "/proc/self/task/%d/%s", (int)a0, (char*)a1);
  }
  int fd = open(buf, O_RDWR);
  if (fd == -1)
    fd = open(buf, O_RDONLY);
  return fd;
}

extern unsigned long long procid;

struct fs_image_segment {
  void* data;
  uintptr_t size;
  uintptr_t offset;
};

#define IMAGE_MAX_SEGMENTS 4096
#define IMAGE_MAX_SIZE (32 << 20)

#define SYZ_memfd_create 319

static uintptr_t syz_mount_image(uintptr_t fs, uintptr_t dir, uintptr_t size,
                                 uintptr_t nsegs, uintptr_t segments,
                                 uintptr_t flags, uintptr_t opts)
{
  char loopname[64];
  int loopfd, err = 0, res = -1;
  uintptr_t i;
  struct fs_image_segment* segs = (struct fs_image_segment*)segments;

  if (nsegs > IMAGE_MAX_SEGMENTS)
    nsegs = IMAGE_MAX_SEGMENTS;
  for (i = 0; i < nsegs; i++) {
    if (segs[i].size > IMAGE_MAX_SIZE)
      segs[i].size = IMAGE_MAX_SIZE;
    segs[i].offset %= IMAGE_MAX_SIZE;
    if (segs[i].offset > IMAGE_MAX_SIZE - segs[i].size)
      segs[i].offset = IMAGE_MAX_SIZE - segs[i].size;
    if (size < segs[i].offset + segs[i].offset)
      size = segs[i].offset + segs[i].offset;
  }
  if (size > IMAGE_MAX_SIZE)
    size = IMAGE_MAX_SIZE;
  int memfd = syscall(SYZ_memfd_create, "syz_mount_image", 0);
  if (memfd == -1) {
    err = errno;
    goto error;
  }
  if (ftruncate(memfd, size)) {
    err = errno;
    goto error_close_memfd;
  }
  for (i = 0; i < nsegs; i++) {
    if (pwrite(memfd, segs[i].data, segs[i].size, segs[i].offset) < 0) {
    }
  }
  snprintf(loopname, sizeof(loopname), "/dev/loop%llu", procid);
  loopfd = open(loopname, O_RDWR);
  if (loopfd == -1) {
    err = errno;
    goto error_close_memfd;
  }
  if (ioctl(loopfd, LOOP_SET_FD, memfd)) {
    if (errno != EBUSY) {
      err = errno;
      goto error_close_loop;
    }
    ioctl(loopfd, LOOP_CLR_FD, 0);
    usleep(1000);
    if (ioctl(loopfd, LOOP_SET_FD, memfd)) {
      err = errno;
      goto error_close_loop;
    }
  }
  mkdir((char*)dir, 0777);
  if (strcmp((char*)fs, "iso9660") == 0)
    flags |= MS_RDONLY;
  if (mount(loopname, (char*)dir, (char*)fs, flags, (char*)opts)) {
    err = errno;
    goto error_clear_loop;
  }
  res = 0;
error_clear_loop:
  ioctl(loopfd, LOOP_CLR_FD, 0);
error_close_loop:
  close(loopfd);
error_close_memfd:
  close(memfd);
error:
  errno = err;
  return res;
}

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
unsigned long long procid;
void execute_call(int call)
{
  long res;
  switch (call) {
  case 0:
    memcpy((void*)0x20000100, "net/netstat", 12);
    res = syz_open_procfs(0, 0x20000100);
    if (res != -1)
      r[0] = res;
    break;
  case 1:
    memcpy((void*)0x20000000, "reiserfs", 9);
    memcpy((void*)0x20000040, "./file0", 8);
    *(uint64_t*)0x20000300 = 0x20000140;
    memcpy((void*)0x20000140,
           "\x09\x21\xa0\x30\x17\x48\x4a\x4c\x50\x45\xb3\xab\x19\x28\x7d\xf7"
           "\xb2\xa3\xd8\xd9\x6a\x9b\x0d\x29\xd7\x71\x52\x78\x3e\x7f\xdb\xe2"
           "\xdd\xce\x93\x4f\x23\xe1\xde\xfe\xea\x26\x46\xf9\x37\xc2\x9a\x21"
           "\xcd\x81\xa6\x6e\x6f\x06\x3b\x5b\xa2\xa8\xd6\x78\x8f\x31\xe2\x06"
           "\x97\x33\x66\x66\x49\x83\xa8\x18\xee\x41\xa0\x8d\x7c\x05\xc7\x55"
           "\xbb\xdc\x4f\xd9\xa5\x27\xd8\x55\xe9\x39\x85\xe8\x87\xde\xe9\xf6"
           "\x64\x6f\x8d\xd1\x8b\x14\x9a\xae\x39\xe4\x7f\x00\xd3\xad\x05\x81"
           "\xd5\xfe\x2e\x6e\x5f\xe6\xa6\x79\x48\x03\xd8\xe5\x38\xe2\x26\x91"
           "\xad\xbe\xab\x2d\x2c\x5c\x53\xe6\x64\x2a\xc1\x2d\x0c\x7d\x47\xa4"
           "\xd1\x21\x73\xdd\xed\xdd\x75\xa6\xbf\xa4\x35\x3a\x1c\x82\x31\x30"
           "\x08\xcc\x69\xd7\xbf\x57\x5c\x7a\x9d\xa7\x2f\xda\x6a\x38\xff\xa6"
           "\x74\x15\x3e\x2d\x43\x9e\x67\xbd\xed\xb9\xaa\x6f\x89\xa2\xe2\x22"
           "\x8f\x7b\x2f\x60\x47\x63\x47\xc9\x48\xc8\x73\xf5\x6f\x1a\xfd\x59"
           "\x99\xf0\x3c\x68\x52\xf5\x36\x94\x58\xf5\x19\xc2\x6c\xf7\xc2\x2b"
           "\x0d\x1c\xd5\xb3\x35\x8d\x25\x30\x61\x84\x6d\xf9\xb9\xf6\xed\xcb"
           "\xf2\xef",
           242);
    *(uint64_t*)0x20000308 = 0xf2;
    *(uint64_t*)0x20000310 = 0x36;
    *(uint64_t*)0x20000318 = 0x200000c0;
    *(uint64_t*)0x20000320 = 0;
    *(uint64_t*)0x20000328 = 0xfffffffffffffffc;
    *(uint64_t*)0x20000330 = 0x20000240;
    *(uint64_t*)0x20000338 = 0;
    *(uint64_t*)0x20000340 = 7;
    *(uint64_t*)0x20000348 = 0x20000280;
    memcpy((void*)0x20000280,
           "\x70\x15\x23\x53\xdf\x78\x11\x59\x91\x60\x1c\xd0\x2e\x87\x01\x81"
           "\x4c\x78\x1e\x83\x47\xf3\xfa\x4f\xbe\x24\xd3\x34\xac\x52\x38\x74"
           "\xf0\x2d\xad\x23\x7f\x80\x58\x2b\x47\x77\x8e\xcf\x13\x55\x06\xbc"
           "\x12\x3b\x6a\x6d\x86\x5e\xc0\xe0\x24\x28\xc5\xdb\x5e\x2a\xb8\xec"
           "\x41\x76\xcc\x93\xd7\xc9\x62\x7c\x87\xe4\x98\xbf\x9a\x0c\xa8\x64"
           "\xd7\xba\x3e\x8d\xac\x45\x37",
           87);
    *(uint64_t*)0x20000350 = 0x57;
    *(uint64_t*)0x20000358 = 2;
    syz_mount_image(0x20000000, 0x20000040, 0x18000, 4, 0x20000300, 0x1000000,
                    0x20000380);
    break;
  case 2:
    syscall(__NR_pread64, r[0], 0x20000080, 0xfef4, 0x1fe);
    break;
  case 3:
    memcpy((void*)0x20000240, "minix", 6);
    memcpy((void*)0x200003c0, "./file0/file0", 14);
    *(uint64_t*)0x20001700 = 0x20000400;
    *(uint64_t*)0x20001708 = 0;
    *(uint64_t*)0x20001710 = 0xcd;
    *(uint64_t*)0x20001718 = 0x20001400;
    *(uint64_t*)0x20001720 = 0;
    *(uint64_t*)0x20001728 = 0xac;
    *(uint64_t*)0x20001730 = 0x20001500;
    *(uint64_t*)0x20001738 = 0;
    *(uint64_t*)0x20001740 = 7;
    *(uint64_t*)0x20001748 = 0x20001540;
    memcpy((void*)0x20001540,
           "\x21\xe2\x1c\x19\x76\x40\xf1\x11\x5b\x1b\x88\x97\x27\xbf\x52\x23"
           "\x9d\xb8\x09\x93\x4c\xa8\x7c\x3c\x85\xaf\x5e\xb9\x11\x3d\xa5\x02"
           "\x82\xc6\xe9\x55\xf1\x95\x04\x2a\x5d\xa5\x69\xef\x33\x5e\xf8\x15"
           "\xe5\x7f\xc2\xa5\xe0\x03\xff\xdf\x76\x70\x70\x7b\xaf\x10\xa1\xbc"
           "\x55\xce\xc8\xbd\x67\xad\x86\x8c\x0e\x4b\x36\xbd\xb2\x5c\xda",
           79);
    *(uint64_t*)0x20001750 = 0x4f;
    *(uint64_t*)0x20001758 = 2;
    *(uint64_t*)0x20001760 = 0x200015c0;
    memcpy((void*)0x200015c0, "\xf7\x20\x1e\x94\x43\x1a\x94\x20\x45\xc9\x4f",
           11);
    *(uint64_t*)0x20001768 = 0xb;
    *(uint64_t*)0x20001770 = 1;
    *(uint64_t*)0x20001778 = 0x20001600;
    *(uint64_t*)0x20001780 = 0;
    *(uint64_t*)0x20001788 = 1;
    *(uint64_t*)0x20001790 = 0x20001680;
    *(uint64_t*)0x20001798 = 0;
    *(uint64_t*)0x200017a0 = 0x8000;
    *(uint64_t*)0x200017a8 = 0x200016c0;
    *(uint64_t*)0x200017b0 = 0;
    *(uint64_t*)0x200017b8 = 7;
    syz_mount_image(0x20000240, 0x200003c0, 0x8001, 8, 0x20001700, 0x80, 0);
    break;
  }
}

void execute_one()
{
  execute(4);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  for (procid = 0; procid < 8; procid++) {
    if (fork() == 0) {
      for (;;) {
        loop();
      }
    }
  }
  sleep(1000000);
  return 0;
}