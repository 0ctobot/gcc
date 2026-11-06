/* { dg-do run } */
/* { dg-options "-O2" } */

/* A pointer stepped out of a member of an object of unknown size by an offset
   that's only known at run time may still be well inside the enclosing
   object, so its maximum size may be larger than what's left, but never
   smaller.  The same goes for a pointer stepped again from there, in either
   direction: a second step back must not wrap the size around to a tiny one.
   The minimum stays at most what's left.  */

typedef __SIZE_TYPE__ size_t;

struct e
{
  char pad[12];
  char nm[20];
};

struct s
{
  long h[2];
  struct e a[3];
};

char buf[256];

__attribute__ ((noipa)) void
use (char *q)
{
  (void) q;
}

#define STEPS(NAME, B, T)						\
  __attribute__ ((noipa)) size_t					\
  NAME (struct s *p, long j, long k)					\
  {									\
    char *q = p->a[0].nm + j;						\
    use (q);								\
    char *r = q + k;							\
    use (r);								\
    return B (r, T);							\
  }

STEPS (d0, __builtin_dynamic_object_size, 0)
STEPS (d1, __builtin_dynamic_object_size, 1)
STEPS (d2, __builtin_dynamic_object_size, 2)
STEPS (d3, __builtin_dynamic_object_size, 3)

#define STEPS3(NAME, T)							\
  __attribute__ ((noipa)) size_t					\
  NAME (struct s *p, long j, long k, long l)				\
  {									\
    char *q = p->a[0].nm + j;						\
    use (q);								\
    char *r = q + k;							\
    use (r);								\
    char *t = r + l;							\
    use (t);								\
    return __builtin_dynamic_object_size (t, T);			\
  }

STEPS3 (t0, 0)
STEPS3 (t1, 1)

/* p->a[0].nm in P is 164 bytes from the end of buf.  */

static void
check (struct s *p, long j, long k)
{
  size_t left = 164 - j - k;

  if (d0 (p, j, k) < left || d1 (p, j, k) < left)
    __builtin_abort ();
  if (d2 (p, j, k) > left || d3 (p, j, k) > left)
    __builtin_abort ();
}

static void
check3 (struct s *p, long j, long k, long l)
{
  size_t left = 164 - j - k - l;

  if (t0 (p, j, k, l) < left || t1 (p, j, k, l) < left)
    __builtin_abort ();
}

int
main (void)
{
  struct s *p = (struct s *) (buf + 64);

  /* Two steps back.  */
  check (p, -16, -8);
  check (p, -1, -1);
  /* Back, then forward.  */
  check (p, -16, 4);
  check (p, -16, 20);
  /* Forward past the end of the member, then back.  */
  check (p, 24, -10);
  check (p, 24, -30);
  /* Back, forward, then back again.  */
  check3 (p, -16, 4, -8);
  /* Steps that stay in the member keep its exact size.  */
  if (d1 (p, 2, -1) != 19)
    __builtin_abort ();
  return 0;
}
