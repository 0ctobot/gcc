/* { dg-do run } */
/* { dg-options "-O2 -fdump-tree-dse1-details" } */

/* Clears through a pointer whose ends are overwritten get trimmed just
   like clears of a local, and the trimmed store covers the right bytes.  */

typedef unsigned long long u64;
struct r { u64 e[16]; };
struct in { u64 q[4]; };
struct out { u64 h; struct in i; u64 t; };

/* memset through a pointer with the last 32 bytes overwritten.  */
__attribute__((noipa)) void
f1 (struct r *p, u64 v)
{
  __builtin_memset (p, 0, sizeof *p);
  p->e[15] = v;
  p->e[14] = v;
  p->e[13] = v;
  p->e[12] = v;
}

/* An aggregate clear through a pointer with the last 32 bytes
   overwritten.  */
__attribute__((noipa)) void
f2 (struct r *p, u64 v)
{
  *p = (struct r){};
  p->e[15] = v;
  p->e[14] = v;
  p->e[13] = v;
  p->e[12] = v;
}

/* An aggregate clear through a pointer with the first 32 bytes
   overwritten.  */
__attribute__((noipa)) void
f3 (struct r *p, u64 v)
{
  *p = (struct r){};
  p->e[0] = v;
  p->e[1] = v;
  p->e[2] = v;
  p->e[3] = v;
}

/* An aggregate clear of a member through a pointer with its first word
   overwritten.  */
__attribute__((noipa)) void
f4 (struct out *o, u64 v)
{
  o->i = (struct in){};
  o->i.q[0] = v;
}

/* An aggregate clear at a constant offset from a pointer with its last
   word overwritten.  */
__attribute__((noipa)) void
f5 (struct in *p, u64 v)
{
  p[1] = (struct in){};
  p[1].q[3] = v;
}

#define V 0x0123456789abcdefULL
#define G 0x5a5a5a5a5a5a5a5aULL

union u { u64 w[32]; struct r r[2]; struct out o; struct in i[8]; } m;

static void
check (int lo, int hi, int vlo, int vhi)
{
  for (int k = 0; k < 32; k++)
    {
      u64 want = G;
      if (k >= vlo && k < vhi)
	want = V;
      else if (k >= lo && k < hi)
	want = 0;
      if (m.w[k] != want)
	__builtin_abort ();
    }
}

int
main (void)
{
  for (int k = 0; k < 32; k++)
    m.w[k] = G;
  f1 (&m.r[1], V);
  check (16, 32, 28, 32);

  for (int k = 0; k < 32; k++)
    m.w[k] = G;
  f2 (&m.r[1], V);
  check (16, 32, 28, 32);

  for (int k = 0; k < 32; k++)
    m.w[k] = G;
  f3 (&m.r[1], V);
  check (16, 32, 16, 20);

  for (int k = 0; k < 32; k++)
    m.w[k] = G;
  f4 (&m.o, V);
  check (1, 5, 1, 2);

  for (int k = 0; k < 32; k++)
    m.w[k] = G;
  f5 (&m.i[2], V);
  check (12, 16, 15, 16);

  return 0;
}

/* { dg-final { scan-tree-dump-times "Trimming statement \\(head = 0, tail = 32\\)" 2 "dse1" } } */
/* { dg-final { scan-tree-dump-times "Trimming statement \\(head = 32, tail = 0\\)" 1 "dse1" } } */
/* { dg-final { scan-tree-dump-times "__builtin_memset \\(p_\[0-9\]+\\(D\\), 0, 96\\);" 1 "dse1" } } */
/* { dg-final { scan-tree-dump-times "MEM <char\\\[96\\\]> \\\[\\(struct r \\*\\)p_\[0-9\]+\\(D\\)\\\] = \{\}" 1 "dse1" } } */
/* { dg-final { scan-tree-dump-times "MEM <char\\\[96\\\]> \\\[\\(struct r \\*\\)p_\[0-9\]+\\(D\\) \\+ 32B\\\] = \{\}" 1 "dse1" } } */
/* { dg-final { scan-tree-dump-times "MEM <char\\\[24\\\]> \\\[\\(struct in \\*\\)o_\[0-9\]+\\(D\\) \\+ 16B\\\] = \{\}" 1 "dse1" } } */
/* { dg-final { scan-tree-dump-times "MEM <char\\\[24\\\]> \\\[\\(struct in \\*\\)p_\[0-9\]+\\(D\\) \\+ 32B\\\] = \{\}" 1 "dse1" } } */
