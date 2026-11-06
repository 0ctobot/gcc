/* PR tree-optimization/114206 */
/* A value that can hold bits of the address of a live local must block a tail
   call, whichever route it takes: integer conversions, arithmetic, memory, a
   call that received the address, or branches on the address.  */
/* { dg-do compile } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -foptimize-sibling-calls -fdump-tree-tailc-details -fdump-tree-tailr1-details" } */

typedef unsigned long ul;
extern long g (long);
extern long g2 (long, long);
extern void esc (void *);
extern ul idp (void *) __attribute__ ((pure));
extern ul idc (void *) __attribute__ ((const));
extern long cf (long) __attribute__ ((const));
extern ul strlen (const char *);
extern int snprintf (char *, ul, const char *, ...);
long glob;
void *gp;
long tab[16];

#define N __attribute__ ((noinline))
long
r_wide (void)
{
  int l;
  return g ((ul) &l);
}

long
r_narrow (void)
{
  int l;
  return g ((unsigned short) (ul) &l);
}

long
r_halves (void)
{
  int l;
  ul v = (ul) &l;
  return g2 (v >> 32, (unsigned int) v);
}

long
r_arith (void)
{
  int l;
  ul v = (ul) &l;
  return g (v * 3 ^ 5);
}

long
r_local_mem (long i)
{
  int l;
  struct { void *p;
  long x[3];
  } s;
  s.p = &l;
  s.x[i] = 1;
  ul *q = (ul *) &s;
  return g (q[i ? 0 : 0]);
}

long
r_mem_int (void)
{
  int l;
  ul s[2];
  s[1] = (ul) &l;
  s[0] = 3;
  return g (s[glob & 1]);
}

long
r_pure (void)
{
  int l;
  ul v = idp (&l);
  return g (v);
}

long
r_const (void)
{
  int l;
  ul v = idc (&l);
  return g (v);
}

static N long
sid (void *p)
{
  return (long) p;
}

long
r_static (void)
{
  int l;
  return g (sid (&l));
}

long
r_index (void)
{
  int l;
  return g (tab[(ul) &l & 15]);
}

long
r_branch (void)
{
  int l;
  ul a = (ul) &l;
  int v = 0;
  for (int i = 0;
  i < 16;
  i++) if (a & (1UL << i)) v |= 1 << i;
  return g (v);
}

long
r_cmp (void)
{
  int l;
  int v = (char *) &l < (char *) gp;
  return g (v);
}

long
r_diff (void)
{
  int l;
  return g ((char *) &l - (char *) gp);
}

long
r_asm_r (void)
{
  int l;
  long x;
  asm ("" : "=r" (x) : "0" (&l));
  return g (x);
}

long
r_asm_m (void)
{
  int l;
  long x;
  asm ("" : "=r" (x) : "m" (l));
  return g (x);
}

long
r_memcpy (void)
{
  int l;
  void *p = &l;
  long x;
  __builtin_memcpy (&x, &p, sizeof x);
  return g (x);
}

long
r_esc (void)
{
  int l;
  esc (&l);
  return g (glob);
}

long
r_esc_const (void)
{
  int l;
  esc (&l);
  return cf (glob);
}

ul
r_builtin (void)
{
  char l;
  esc (&l);
  return strlen ((char *) glob);
}

long
r_bitfield (void)
{
  int l;
  union { void *p;
  struct { unsigned a : 13;
  } b;
  } u;
  u.p = &l;
  return g (u.b.a);
}

long
r_phi (int c)
{
  int l;
  ul v = c ? (ul) &l : 7;
  return g (v);
}

long
r_ptr_int (void)
{
  int l;
  int *p = &l + 1;
  return g ((long) p);
}

static N long
sdiff (char *p)
{
  return p - (char *) gp;
}

long
r_sdiff (void)
{
  char l;
  return g (sdiff (&l));
}

static N int
sbit (void *p)
{
  return ((ul) p & 16) != 0;
}

long
r_sbit (void)
{
  int l;
  return g (sbit (&l));
}

static N int
sbr (void *p)
{
  if ((ul) p & 16) return 5;
  return 3;
}

long
r_sbr (void)
{
  int l;
  int v = sbr (&l);
  glob = v;
  return g (v);
}

static N void
sid2 (long *o, void *p)
{
  *o = (long) p;
}

long
r_out (void)
{
  int l;
  long x;
  sid2 (&x, &l);
  return g (x);
}

long
r_snprintf (void)
{
  int l;
  char b[32];
  snprintf (b, 32, "%p", &l);
  return g (b[5]);
}

/* The bits reach the next activation through branches alone.  */
N int
r_rec (int n, unsigned int lo)
{
  int local = n;
  ul me = (ul) &local;
  unsigned int piece = 0;
  for (int i = 0; i < 32; i++)
    if (me & (1ul << i))
      piece |= 1u << i;
  if (lo == piece)
    return -1;
  if (n == 0)
    return 0;
  return r_rec (n - 1, piece);
}

/* { dg-final { scan-tree-dump-not "Found tail call" "tailc" } } */
/* { dg-final { scan-tree-dump-not "Eliminated tail recursion" "tailr1" } } */
