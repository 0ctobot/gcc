/* { dg-do run } */
/* { dg-options "-O2" } */

/* A pointer stepped past the end of a member of an object of unknown size by
   an offset that's only known at run time, and then back into the member, is
   in the member again.  Its maximum size must be at least what's left of the
   member, but no more than the size of the member, where it used to be one
   that no object can have.  A pointer that's still past the end of the member
   gets 0, like one that a single step takes past it.  */

typedef __SIZE_TYPE__ size_t;

struct e
{
  int tag;
  char nm[24];
  short z;
};

struct s
{
  char hd[8];
  struct e a[4];
};

char buf[512];

__attribute__ ((noipa)) void
use (char *q)
{
  (void) q;
}

__attribute__ ((noipa)) size_t
steps (struct s *p, long j, long k)
{
  char *q = &p->a[2].nm[3] + j;
  use (q);
  char *r = q + k;
  use (r);
  char *t = r + k;
  use (t);
  return __builtin_dynamic_object_size (t, 1);
}

__attribute__ ((noipa)) size_t
loop (struct s *p, long j, long k, int n)
{
  char *q = &p->a[2].nm[3] + j;
  use (q);
  for (int i = 0; i < n; i++)
    {
      q += k;
      use (q);
    }
  return __builtin_dynamic_object_size (q, 1);
}

/* The pointer ends up 3 + J + N * K bytes into p->a[2].nm.  */

static void
check (size_t sz, long j, long k, int n)
{
  size_t left = 24 - (3 + j + n * k);

  if (sz < left || sz > 24)
    __builtin_abort ();
}

int
main (void)
{
  struct s *p = (struct s *) (buf + 160);

  /* Past the end of the member, then back into it.  */
  check (steps (p, 24, -12), 24, -12, 2);
  check (steps (p, 22, -11), 22, -11, 2);
  check (steps (p, 21, -9), 21, -9, 2);
  check (steps (p, 21, -12), 21, -12, 2);
  /* Steps that stay in the member.  */
  check (steps (p, 20, -10), 20, -10, 2);
  check (steps (p, 10, -5), 10, -5, 2);
  check (steps (p, 0, 0), 0, 0, 2);
  /* The same in a loop.  */
  check (loop (p, 24, -6, 4), 24, -6, 4);
  check (loop (p, 30, -3, 9), 30, -3, 9);
  check (loop (p, 0, 0, 3), 0, 0, 3);
  /* Still past the end of the member.  */
  if (steps (p, 30, -2) != 0)
    __builtin_abort ();
  return 0;
}
