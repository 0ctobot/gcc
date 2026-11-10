/* { dg-do compile } */
/* { dg-options "-O2" } */

/* A store to a volatile bit-field that exactly fills a byte, halfword or
   word mustn't read that location first, since none of its bits need to be
   preserved and the read is an access the source doesn't make.  The
   declared-type container can't be used for these because it would
   overlap the following non-bit-field member.  Only f6, whose two fields
   share their container, needs a read-modify-write.  The fields of f7 and
   f8 start at an odd byte, so their single halfword and word stores are
   unaligned; they mustn't be split into narrower stores either.  f9 and
   f10 store constants into such fields, which mustn't be split or read
   either.  */

struct s1 { volatile unsigned a:8; unsigned char c; unsigned short d; };
struct s2 { unsigned a:8; unsigned char c; unsigned short d; };
struct s3 { volatile unsigned a:16; unsigned short d; };
struct s4 { unsigned char c; volatile unsigned a:8; unsigned short d; };
struct s5 { unsigned long a:32; unsigned b; };
struct s6 { volatile unsigned a:16; volatile unsigned b:16; };
struct s7 { char c; volatile unsigned a:16; char d; };
struct s8 { char c; volatile unsigned long a:32; char d; };
struct s9 { char c; volatile int a:16; char d; };

void f1 (struct s1 *p) { p->a = 1; }
void f2 (volatile struct s2 *p, int x) { p->a = x; }
void f3 (struct s3 *p, int x) { p->a = x; }
void f4 (struct s4 *p, int x) { p->a = x; }
void f5 (volatile struct s5 *p, int x) { p->a = x; }
void f6 (struct s6 *p, int x) { p->a = x; }
void f7 (struct s7 *p, int x) { p->a = x; }
void f8 (struct s8 *p, int x) { p->a = x; }
void f9 (struct s9 *p) { p->a = -2; }
void f10 (struct s8 *p) { p->a = 0x12345678; }

/* { dg-final { scan-assembler-times {\tldr[bh]?\t} 1 } } */
/* { dg-final { scan-assembler-times {\tldr\tw[0-9]+, \[x0\]} 1 } } */
/* { dg-final { scan-assembler-times {\tstrh\tw[0-9]+, \[x0, 1\]} 2 } } */
/* { dg-final { scan-assembler-times {\tstr\tw[0-9]+, \[x0, 1\]} 2 } } */
/* { dg-final { scan-assembler-not {\tstrb\tw[0-9]+, \[x0, [234]\]} } } */
