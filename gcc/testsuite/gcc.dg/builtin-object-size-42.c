/* { dg-do run } */
/* { dg-options "-O2" } */

/* A member reached through a pointer of unknown size, with an index into
   the member that may be negative, has a dynamic size that's chosen at run
   time: the member's size when the index isn't negative, and a size at least
   as large as what's really left when it is, since the pointer may still be
   well inside the enclosing object.  The same goes for a pointer stepped from
   such a member, in either direction.  The minimum stays at most what's
   left.  */

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

__attribute__ ((noipa)) size_t
inner (struct s *p, int n, int type)
{
  if (type == 1)
    return __builtin_dynamic_object_size (&p->a[2].nm[n], 1);
  return __builtin_dynamic_object_size (&p->a[2].nm[n], 3);
}

__attribute__ ((noipa)) size_t
inner_step (struct s *p, int n, long k)
{
  char *q = &p->a[2].nm[n];
  use (q);
  char *r = q + k;
  use (r);
  return __builtin_dynamic_object_size (r, 1);
}

/* What's really left in BUF from P->a[I].nm[N].  */
#define LEFT(i, n)							\
  (sizeof (buf) - 64 - __builtin_offsetof (struct s, a[0].nm) - 32 * (i) - (n))

int
main (void)
{
  struct s *p = (struct s *) (buf + 64);

  if (inner (p, 0, 1) != 20 || inner (p, 3, 1) != 17
      || inner (p, -1, 1) < LEFT (2, -1))
    __builtin_abort ();
  if (inner (p, 0, 3) != 20 || inner (p, 3, 3) != 17
      || inner (p, -1, 3) > LEFT (2, -1))
    __builtin_abort ();
  if (inner_step (p, 3, 4) != 13 || inner_step (p, -1, 4) < LEFT (2, 3)
      || inner_step (p, -1, -8) < LEFT (2, -9))
    __builtin_abort ();
  return 0;
}
