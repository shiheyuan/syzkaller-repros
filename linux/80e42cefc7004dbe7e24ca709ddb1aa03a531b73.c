// fatal error: runtime: out of memory
// https://syzkaller.appspot.com/bug?id=80e42cefc7004dbe7e24ca709ddb1aa03a531b73
// status:invalid
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

#define BITMASK_LEN(type, bf_len) (type)((1ull << (bf_len)) - 1)

#define BITMASK_LEN_OFF(type, bf_off, bf_len)                                  \
  (type)(BITMASK_LEN(type, (bf_len)) << (bf_off))

#define STORE_BY_BITMASK(type, addr, val, bf_off, bf_len)                      \
  if ((bf_off) == 0 && (bf_len) == 0) {                                        \
    *(type*)(addr) = (type)(val);                                              \
  } else {                                                                     \
    type new_val = *(type*)(addr);                                             \
    new_val &= ~BITMASK_LEN_OFF(type, (bf_off), (bf_len));                     \
    new_val |= ((type)(val)&BITMASK_LEN(type, (bf_len))) << (bf_off);          \
    *(type*)(addr) = new_val;                                                  \
  }

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

static void execute_one();
extern unsigned long long procid;

void loop()
{
  while (1) {
    execute_one();
  }
}

#ifndef __NR_getrandom
#define __NR_getrandom 318
#endif

uint64_t r[7] = {0x0,
                 0xffffffffffffffff,
                 0xffffffffffffffff,
                 0x0,
                 0xffffffffffffffff,
                 0xffffffffffffffff,
                 0xffffffffffffffff};
void execute_one()
{
  long res = 0;
  syscall(__NR_sysinfo, 0x20000140);
  memcpy((void*)0x20000200, "big_key", 8);
  *(uint8_t*)0x20000240 = 0x73;
  *(uint8_t*)0x20000241 = 0x79;
  *(uint8_t*)0x20000242 = 0x7a;
  *(uint8_t*)0x20000243 = 0x22;
  *(uint8_t*)0x20000244 = 0;
  syscall(__NR_add_key, 0x20000200, 0x20000240, 0x20000280, 0, 0xfffffffb);
  *(uint32_t*)0x200003c0 = 8;
  *(uint32_t*)0x200003c4 = -1;
  *(uint32_t*)0x200003c8 = 0x3400;
  STORE_BY_BITMASK(uint32_t, 0x200003cc, 6, 0, 1);
  STORE_BY_BITMASK(uint32_t, 0x200003cc, 3, 1, 2);
  STORE_BY_BITMASK(uint32_t, 0x200003cc, 3, 3, 1);
  STORE_BY_BITMASK(uint32_t, 0x200003cc, 3, 4, 1);
  STORE_BY_BITMASK(uint32_t, 0x200003cc, 0x3f, 5, 1);
  STORE_BY_BITMASK(uint32_t, 0x200003cc, 0xe6d, 6, 1);
  STORE_BY_BITMASK(uint32_t, 0x200003cc, 0, 7, 1);
  syscall(__NR_modify_ldt, 1, 0x200003c0, 0x10);
  *(uint32_t*)0x20000440 = 0xc;
  res = syscall(__NR_getsockopt, -1, 1, 0x11, 0x20000400, 0x20000440);
  if (res != -1)
    r[0] = *(uint32_t*)0x20000400;
  memcpy((void*)0x200006c0, "syz1", 4);
  syscall(__NR_write, -1, 0x200006c0, 4);
  memcpy((void*)0x20001700, "syz0", 5);
  syscall(__NR_openat, -1, 0x20001700, 0x200002, 0);
  memcpy((void*)0x20001780, "tasks", 6);
  syscall(__NR_openat, -1, 0x20001780, 2, 0);
  syscall(__NR_tgkill, r[0], r[0], 0x32);
  memcpy((void*)0x20000100, "/dev/null", 10);
  syscall(__NR_openat, 0xffffffffffffff9c, 0x20000100, 0x802, 0);
  memcpy((void*)0x20000000, "./file0", 8);
  syscall(__NR_creat, 0x20000000, 2);
  *(uint32_t*)0x20001140 = 0;
  *(uint16_t*)0x20001148 = 2;
  *(uint16_t*)0x2000114a = htobe16(0);
  *(uint8_t*)0x2000114c = 0xac;
  *(uint8_t*)0x2000114d = 0x14;
  *(uint8_t*)0x2000114e = 0x14;
  *(uint8_t*)0x2000114f = 0xaa;
  *(uint8_t*)0x20001150 = 0;
  *(uint8_t*)0x20001151 = 0;
  *(uint8_t*)0x20001152 = 0;
  *(uint8_t*)0x20001153 = 0;
  *(uint8_t*)0x20001154 = 0;
  *(uint8_t*)0x20001155 = 0;
  *(uint8_t*)0x20001156 = 0;
  *(uint8_t*)0x20001157 = 0;
  *(uint16_t*)0x200011c8 = 2;
  *(uint16_t*)0x200011ca = htobe16(0);
  *(uint32_t*)0x200011cc = htobe32(0xe0000002);
  *(uint8_t*)0x200011d0 = 0;
  *(uint8_t*)0x200011d1 = 0;
  *(uint8_t*)0x200011d2 = 0;
  *(uint8_t*)0x200011d3 = 0;
  *(uint8_t*)0x200011d4 = 0;
  *(uint8_t*)0x200011d5 = 0;
  *(uint8_t*)0x200011d6 = 0;
  *(uint8_t*)0x200011d7 = 0;
  syscall(__NR_setsockopt, -1, 0, 0, 0x20001140, 0xffffffffffffff30);
  syscall(__NR_getrandom, 0x20000000, 0x51, 3);
  syscall(__NR_mmap, 0x20000000, 0x7fffdffff000, 0, 0x31, -1, 0);
  res = syscall(__NR_socketpair, 2, 2, 0, 0x20000000);
  if (res != -1)
    r[1] = *(uint32_t*)0x20000004;
  syscall(__NR_fcntl, r[1], 0xb);
  syscall(__NR_shmget, 0, 0x4000, 0x800, 0x20ff9000);
  memcpy((void*)0x20000080, "/dev/zero", 10);
  syscall(__NR_openat, 0xffffffffffffff9c, 0x20000080, 0x40, 0);
  syscall(__NR_socket, 0x10, 3, 0);
  memcpy((void*)0x200000c0, "./file0", 8);
  memcpy((void*)0x20000100, "./file0", 8);
  syscall(__NR_linkat, -1, 0x200000c0, -1, 0x20000100, 0x1400);
  memcpy((void*)0x20000000, "cpuacct.usage_percpu", 21);
  syscall(__NR_openat, -1, 0x20000000, 0, 0);
  res = syscall(__NR_socket, 0x10, 3, 0);
  if (res != -1)
    r[2] = res;
  syscall(__NR_fcntl, r[2], 9);
  syscall(__NR_ioctl, -1, 0x5429, 0x20002440);
  syscall(__NR_getgid);
  memcpy((void*)0x20002a80, "./file0", 8);
  syscall(__NR_lstat, 0x20002a80, 0x20002ac0);
  *(uint16_t*)0x20000040 = 0;
  *(uint16_t*)0x20000042 = 0;
  *(uint32_t*)0x20000044 = 0;
  *(uint32_t*)0x20000048 = 0;
  syscall(__NR_connect, -1, 0x20000040, 0xc);
  syscall(__NR_io_destroy, 0);
  syscall(__NR_fstat, -1, 0x200001c0);
  *(uint32_t*)0x20000180 = 0xe8;
  res = syscall(__NR_getsockopt, -1, 0, 0x10, 0x20000080, 0x20000180);
  if (res != -1)
    r[3] = *(uint32_t*)0x200000b0;
  *(uint32_t*)0x20000240 = r[3];
  *(uint8_t*)0x20000244 = 0xac;
  *(uint8_t*)0x20000245 = 0x14;
  *(uint8_t*)0x20000246 = 0x14;
  *(uint8_t*)0x20000247 = 0xbb;
  *(uint32_t*)0x20000248 = htobe32(-1);
  syscall(__NR_setsockopt, -1, 0, 8, 0x20000240, 0xc);
  *(uint32_t*)0x20000200 = 0xc;
  syscall(__NR_getsockopt, -1, 1, 0x11, 0x200001c0, 0x20000200);
  *(uint32_t*)0x200000c0 = 0;
  res = syscall(__NR_accept, 0xffffff9c, 0, 0x200000c0);
  if (res != -1)
    r[4] = res;
  res = syscall(__NR_dup, 0xffffff9c);
  if (res != -1)
    r[5] = res;
  *(uint64_t*)0x200004c0 = 0x20000200;
  *(uint64_t*)0x20000200 = 0;
  *(uint32_t*)0x20000208 = 0;
  *(uint32_t*)0x2000020c = 0;
  *(uint16_t*)0x20000210 = 0x5396;
  *(uint16_t*)0x20000212 = 0x3ff;
  *(uint32_t*)0x20000214 = r[4];
  *(uint64_t*)0x20000218 = 0x20000100;
  memcpy((void*)0x20000100,
         "\xc8\x8b\x5a\x61\xe6\x27\x31\xed\x29\x7f\x20\x88\x4a\x83\x61\xff\x76"
         "\x99\x2d\x3e\x36\xcf\xe6\xdc\xf5\xfe\xcb\x53\xa7\x8f\x26\xb2\xb9\x6f"
         "\x6f\x1d\xe5\x31\x56\x7c\x01\x29\xbc\xfc\xf5\xe6\xf6\xf3\x46\x1a\xda"
         "\xc0\xbe\xf5\xa1\x17\xa0\xe6\xf1\xdf\x8d\x95\x71\xb0\xdd\xbf\x9c\xaf"
         "\x37\x48\x13\x01\xbb\x34\x1a\x56\xf5\x13\xba\xa4\x3d\x69\xc7\x31\x34"
         "\xbe\x42\x30\x0e\x0e\xc3\x37\x01\x71\xf9\x87\x29\xd5\x18\x59\x67\x02"
         "\x09\x0e\x99\xaf\xc8\xf8\x4c\x67\xea\x8d\xf9\x17\xbe\x30\x8f\xc9\xb5"
         "\xf5\x29\x62\x9b\xd6\x6e\x6f\xcb\xfe\xd7\xf7\xf3\xa0\x4a\x3c\x10\xd9"
         "\x85\xc7\x97\x97\x2d\x3d\x41\xdf\x1a\xe3\xc4\x1f\xeb\x68\x45\x3c\xce"
         "\xfd\xc1\x86\x91\x70\x41\x07\xba\x5d\xc2\xbc\x85\xec\xf4\x06\xd1\xe6"
         "\xc1\x11\xa6\x2a\xc8\x33\xa0\x7c\xeb\x82\xc1\xbc\xeb\xc2\x97\xc0\xa5"
         "\x4f\x6f\x24\x5c\x8f\x85\xf6\x69\xc0\xd1\x01\x7c\xdf\x43\xef\xbe\x4c"
         "\x07\x7d\xde\x57\x6c\xee\xb6\x0c\x7d\xa4\x06\x8a\x1f\x78\x48\xd0\xb7"
         "\x7c\xb2\x8e\xc5\xf5\x8a\x54\xa2\x05\x84\xa1\x84\x36\x5b\x8e\x0a\x07"
         "\x31\x93\x43\xc7\xf1\x77\xf3\x08\xfc\x6b\x20\x79\x07\x2d",
         252);
  *(uint64_t*)0x20000220 = 0xfc;
  *(uint64_t*)0x20000228 = 8;
  *(uint64_t*)0x20000230 = 0;
  *(uint32_t*)0x20000238 = 0;
  *(uint32_t*)0x2000023c = r[5];
  *(uint64_t*)0x200004c8 = 0x200002c0;
  *(uint64_t*)0x200002c0 = 0;
  *(uint32_t*)0x200002c8 = 0;
  *(uint32_t*)0x200002cc = 0;
  *(uint16_t*)0x200002d0 = 0xd;
  *(uint16_t*)0x200002d2 = 7;
  *(uint32_t*)0x200002d4 = r[4];
  *(uint64_t*)0x200002d8 = 0x20000240;
  *(uint64_t*)0x200002e0 = 0;
  *(uint64_t*)0x200002e8 = 0x7fffffff;
  *(uint64_t*)0x200002f0 = 0;
  *(uint32_t*)0x200002f8 = 0;
  *(uint32_t*)0x200002fc = r[5];
  *(uint64_t*)0x200004d0 = 0x20000380;
  *(uint64_t*)0x20000380 = 0;
  *(uint32_t*)0x20000388 = 0;
  *(uint32_t*)0x2000038c = 0;
  *(uint16_t*)0x20000390 = 1;
  *(uint16_t*)0x20000392 = 0xf800;
  *(uint32_t*)0x20000394 = r[4];
  *(uint64_t*)0x20000398 = 0x20000300;
  *(uint64_t*)0x200003a0 = 0;
  *(uint64_t*)0x200003a8 = 8;
  *(uint64_t*)0x200003b0 = 0;
  *(uint32_t*)0x200003b8 = 1;
  *(uint32_t*)0x200003bc = r[5];
  *(uint64_t*)0x200004d8 = 0x20000480;
  *(uint64_t*)0x20000480 = 0;
  *(uint32_t*)0x20000488 = 0;
  *(uint32_t*)0x2000048c = 0;
  *(uint16_t*)0x20000490 = 0;
  *(uint16_t*)0x20000492 = 0xe6;
  *(uint32_t*)0x20000494 = r[5];
  *(uint64_t*)0x20000498 = 0x200003c0;
  memcpy((void*)0x200003c0,
         "\xff\x4f\x90\x7c\xbc\x54\xde\x3a\x79\xc4\x12\x87\x6d\x80\x0d\x54\x12"
         "\x2d\x8c\xd0\xb6\x64\xe1\xf8\x13\x08\x04\x2b\x18\xb3\x8f\x4b\x0f\x18"
         "\x22\xb0\x62\x0b\x70\x07\xea\x19\xba\xfb\xb2\xbf\x65\x41\x0d\x6a\xd8"
         "\x9e\xb2\x06\xfe\xb2\x7d\xe5\x90\xa9\xae\x0d\xf1\x1c\x6b\xa1\xfb\x13"
         "\x65\x90\x0d\xaf\xd9\x2b\x50\xfc\x03\xf5\x9c\xd6\x89\x0d\x5d\x6b\xd1"
         "\xd1\x77\xeb\xd6\x05\xa2\xb9\x83\xe8\xaa\x78\x49\xa4\x71\x90\xc8\xf9"
         "\x78\x44\x3f\xcf\xdf\x36\xf0\x07\xdb\x93\x7f\xae\x4b\x87\xc5\xc8\xfb"
         "\x80\x5d\x9a\x26\xad\xc6\xc6\x4e\x72\xa9\xdd\x6c\x47\xff\xa9\x80\x5c"
         "\x54\x55\xbc\x62\xe5\xb6\x18\xb4\x63\x01\xc1\xa8\x5b\x9b\x67\x78\xe6"
         "\x57\x21\xeb\xdd\xb7\x9e\x57\x79\x56\x47\x83\x3d\xd6\x7b\x94\x74\x66"
         "\xd3\xc3\xee\xf9\x49\x33\x26",
         177);
  *(uint64_t*)0x200004a0 = 0xb1;
  *(uint64_t*)0x200004a8 = 0x8000;
  *(uint64_t*)0x200004b0 = 0;
  *(uint32_t*)0x200004b8 = 0;
  *(uint32_t*)0x200004bc = r[5];
  syscall(__NR_io_submit, 0, 4, 0x200004c0);
  syscall(__NR_io_submit, 0, 0xffffffffffffffd5, 0x20000180);
  memcpy((void*)0x20000080,
         "\x74\x65\x61\x6d\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
         16);
  *(uint16_t*)0x20000090 = 2;
  *(uint16_t*)0x20000092 = htobe16(0x4e21);
  *(uint32_t*)0x20000094 = htobe32(0x7f000001);
  *(uint8_t*)0x20000098 = 0;
  *(uint8_t*)0x20000099 = 0;
  *(uint8_t*)0x2000009a = 0;
  *(uint8_t*)0x2000009b = 0;
  *(uint8_t*)0x2000009c = 0;
  *(uint8_t*)0x2000009d = 0;
  *(uint8_t*)0x2000009e = 0;
  *(uint8_t*)0x2000009f = 0;
  syscall(__NR_ioctl, -1, 0x8916, 0x20000080);
  res = syscall(__NR_fcntl, -1, 0, -1);
  if (res != -1)
    r[6] = res;
  *(uint32_t*)0x20000180 = 0;
  *(uint16_t*)0x20000188 = 0xa;
  *(uint16_t*)0x2000018a = htobe16(0);
  *(uint32_t*)0x2000018c = 0;
  *(uint8_t*)0x20000190 = -1;
  *(uint8_t*)0x20000191 = 1;
  *(uint8_t*)0x20000192 = 0;
  *(uint8_t*)0x20000193 = 0;
  *(uint8_t*)0x20000194 = 0;
  *(uint8_t*)0x20000195 = 0;
  *(uint8_t*)0x20000196 = 0;
  *(uint8_t*)0x20000197 = 0;
  *(uint8_t*)0x20000198 = 0;
  *(uint8_t*)0x20000199 = 0;
  *(uint8_t*)0x2000019a = 0;
  *(uint8_t*)0x2000019b = 0;
  *(uint8_t*)0x2000019c = 0;
  *(uint8_t*)0x2000019d = 0;
  *(uint8_t*)0x2000019e = 0;
  *(uint8_t*)0x2000019f = 1;
  *(uint32_t*)0x200001a0 = 0;
  *(uint16_t*)0x20000208 = 0xa;
  *(uint16_t*)0x2000020a = htobe16(0);
  *(uint32_t*)0x2000020c = 0;
  *(uint8_t*)0x20000210 = 0;
  *(uint8_t*)0x20000211 = 0;
  *(uint8_t*)0x20000212 = 0;
  *(uint8_t*)0x20000213 = 0;
  *(uint8_t*)0x20000214 = 0;
  *(uint8_t*)0x20000215 = 0;
  *(uint8_t*)0x20000216 = 0;
  *(uint8_t*)0x20000217 = 0;
  *(uint8_t*)0x20000218 = 0;
  *(uint8_t*)0x20000219 = 0;
  *(uint8_t*)0x2000021a = -1;
  *(uint8_t*)0x2000021b = -1;
  *(uint32_t*)0x2000021c = htobe32(0xe0000002);
  *(uint32_t*)0x20000220 = 0;
  syscall(__NR_setsockopt, r[6], 0x29, 0, 0x20000180, 0x108);
  *(uint32_t*)0x20000080 = 6;
  *(uint32_t*)0x20000084 = 2;
  *(uint32_t*)0x20000088 = 0;
  *(uint32_t*)0x2000008c = 0x835d;
  *(uint8_t*)0x20000090 = 9;
  *(uint8_t*)0x20000091 = 3;
  *(uint8_t*)0x20000092 = 0x93;
  *(uint8_t*)0x20000093 = 8;
  *(uint32_t*)0x20000094 = -1;
  *(uint32_t*)0x20000098 = 8;
  *(uint32_t*)0x2000009c = 0x80;
  *(uint32_t*)0x200000a0 = 9;
  syscall(__NR_ioctl, -1, 0x5404, 0x20000080);
  *(uint32_t*)0x20000100 = 0xc;
  syscall(__NR_getsockopt, -1, 0, 8, 0x200000c0, 0x20000100);
  syscall(__NR_lseek, -1, 1, 0);
  syscall(__NR_ioctl, -1, 0x5411, 0x20000040);
  memcpy((void*)0x20000080, "net/ip_tables_targets", 22);
  syz_open_procfs(0, 0x20000080);
  syscall(__NR_getrusage, 1, 0x20000080);
  syscall(__NR_io_destroy, 0);
  syscall(__NR_io_setup, 3, 0x20000000);
  *(uint32_t*)0x20000500 = 0x57;
  *(uint32_t*)0x20000504 = 8;
  *(uint32_t*)0x20000508 = 1;
  *(uint32_t*)0x2000050c = 0x81;
  *(uint8_t*)0x20000510 = 0x21;
  *(uint8_t*)0x20000511 = 0xd8;
  *(uint8_t*)0x20000512 = -1;
  *(uint8_t*)0x20000513 = -1;
  *(uint32_t*)0x20000514 = 0x20;
  *(uint32_t*)0x20000518 = 0xbf6d;
  *(uint32_t*)0x2000051c = 1;
  *(uint32_t*)0x20000520 = 0xad9c;
  syscall(__NR_ioctl, -1, 0x5403, 0x20000500);
  *(uint8_t*)0x20000600 = 0xfe;
  *(uint8_t*)0x20000601 = 0x80;
  *(uint8_t*)0x20000602 = 0;
  *(uint8_t*)0x20000603 = 0;
  *(uint8_t*)0x20000604 = 0;
  *(uint8_t*)0x20000605 = 0;
  *(uint8_t*)0x20000606 = 0;
  *(uint8_t*)0x20000607 = 0;
  *(uint8_t*)0x20000608 = 0;
  *(uint8_t*)0x20000609 = 0;
  *(uint8_t*)0x2000060a = 0;
  *(uint8_t*)0x2000060b = 0;
  *(uint8_t*)0x2000060c = 0;
  *(uint8_t*)0x2000060d = 0;
  *(uint8_t*)0x2000060e = 0;
  *(uint8_t*)0x2000060f = 0x1f;
  *(uint32_t*)0x20000610 = 0;
  syscall(__NR_setsockopt, -1, 0x29, 0x15, 0x20000600, 0x14);
  memcpy((void*)0x20000080, "tls", 4);
  syscall(__NR_setsockopt, -1, 6, 0x1f, 0x20000080, 4);
  *(uint64_t*)0x200001c0 = 0;
  *(uint64_t*)0x200001c8 = 0;
  syscall(__NR_io_getevents, 0, 0xfffffffbffff1001, 0x7fffffffffffc5d,
          0x20000140, 0x200001c0);
  syscall(__NR_io_setup, 0, 0x200000c0);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  for (;;) {
    loop();
  }
}
