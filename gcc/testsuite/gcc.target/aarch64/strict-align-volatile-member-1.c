/* Reading a misaligned member of a packed record mustn't load a volatile
   member next to it.  */
/* { dg-do compile } */
/* { dg-options "-O2 -mstrict-align" } */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct __attribute__ ((packed, aligned (8))) s1
{
  volatile u8 v;
  u32 w;
  char pad[3];
};
u32 f1 (struct s1 *p) { return p->w; }

struct __attribute__ ((packed, aligned (8))) s2
{
  volatile u8 v;
  u32 a[1];
  char pad[3];
};
u32 f2 (struct s2 *p) { return p->a[0]; }
u32 f3 (struct s2 *p, int i) { return p->a[i]; }

struct __attribute__ ((packed, aligned (4))) s3
{
  u8 c;
  u16 h;
  volatile u8 v;
};
u16 f4 (struct s3 *p) { return p->h; }

/* { dg-final { scan-assembler-not {\tldrh?\t} } } */
/* { dg-final { scan-assembler-not {\tldrb\tw[0-9]+, \[x[0-9]+\]\n} } } */
/* { dg-final { scan-assembler-times {\tldrb\t} 14 } } */
