/* { dg-do run } */
/* { dg-options "-O2" } */

/* A member reached through a pointer of unknown size, with an index into an
   array of structures on the way to the member that may be negative, has a
   dynamic size that's chosen at run time: the member's size when the index
   isn't negative, and a size at least as large as what's really left when it
   is, since the pointer may still be well inside the enclosing object.  The
   same goes for a pointer stepped from such a member, in either direction.
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

__attribute__ ((noipa)) size_t
outer (struct s *p, int i, int type)
{
  if (type == 1)
    return __builtin_dynamic_object_size (&p->a[i].nm, 1);
  return __builtin_dynamic_object_size (&p->a[i].nm, 3);
}

__attribute__ ((noipa)) size_t
outer_step (struct s *p, int i, long k)
{
  char *q = p->a[i].nm;
  use (q);
  char *r = q + k;
  use (r);
  return __builtin_dynamic_object_size (r, 1);
}

/* What's really left in BUF from P->a[I].nm[N].  */
#define LEFT(i, n) (sizeof (buf) - 64 - 16 - 12 - 32 * (i) - (n))

int
main (void)
{
  struct s *p = (struct s *) (buf + 64);

  if (outer (p, 0, 1) != 20 || outer (p, 1, 1) != 20
      || outer (p, -1, 1) < LEFT (-1, 0))
    __builtin_abort ();
  if (outer (p, 0, 3) != 20 || outer (p, 1, 3) != 20
      || outer (p, -1, 3) > LEFT (-1, 0))
    __builtin_abort ();
  if (outer_step (p, 0, 4) != 16 || outer_step (p, -1, 4) < LEFT (-1, 4)
      || outer_step (p, -1, -8) < LEFT (-1, -8))
    __builtin_abort ();
  return 0;
}
