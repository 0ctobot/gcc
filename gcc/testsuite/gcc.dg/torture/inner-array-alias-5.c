/* An address formed with a variable index into a flexible, [1] or [0]
   array that isn't the last member of its structure can point into the
   members that follow it.  A pointer stored through such an address and
   loaded back from a later member must be seen to point to what was
   stored.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -fno-strict-aliasing" } */

struct flat { long n; int *e[1]; int *more[8]; };
struct hdr { long n; int *e[]; };
struct box { struct hdr h; int *more[8]; };
struct mk { long n; int *mark[0]; int *more[8]; };
int x, y;

/* Each function stores &x into more[0] through the array, then stores 2
   through the pointer loaded from more[0], which must change x.  */

__attribute__((noipa)) int
g_one (long i, long j, int c)
{
  struct flat b, b2;
  __builtin_memset (&b, 0, sizeof b);
  __builtin_memset (&b2, 0, sizeof b2);
  b.more[0] = &y;
  b2.more[0] = &y;
  int **q = c ? &b.e[i] : &b2.e[j];
  *q = &x;
  int *r = b.more[0];
  x = 1;
  *r = 2;
  return x;
}

__attribute__((noipa)) int
g_flex (long i, long j, int c)
{
  struct box b, b2;
  __builtin_memset (&b, 0, sizeof b);
  __builtin_memset (&b2, 0, sizeof b2);
  b.more[0] = &y;
  b2.more[0] = &y;
  int **q = c ? &b.h.e[i] : &b2.h.e[j];
  *q = &x;
  int *r = b.more[0];
  x = 1;
  *r = 2;
  return x;
}

__attribute__((noipa)) int
g_zero (long i, long j, int c)
{
  struct mk b, b2;
  __builtin_memset (&b, 0, sizeof b);
  __builtin_memset (&b2, 0, sizeof b2);
  b.more[0] = &y;
  b2.more[0] = &y;
  int **q = c ? &b.mark[i] : &b2.mark[j];
  *q = &x;
  int *r = b.more[0];
  x = 1;
  *r = 2;
  return x;
}

int
main (void)
{
  if (g_one (1, 1, 1) != 2)
    __builtin_abort ();
  if (g_flex (0, 0, 1) != 2)
    __builtin_abort ();
  if (g_zero (0, 0, 1) != 2)
    __builtin_abort ();
  return 0;
}
