/* { dg-do run } */
/* { dg-options "-O2" } */

/* A member reached through a pointer of unknown size, with an index into an
   array of structures on the way to the member that may be negative, has a
   dynamic size that's chosen at run time: the member's size when the index
   isn't negative, and a size at least as large as what's really left when it
   is, since the pointer may still be well inside the enclosing object.  The
   same goes for a pointer stepped from such a member, in either direction and
   by a constant or a variable amount, including through a PHI.  The minimum
   stays at most what's left and keeps the member's size when that's known to
   be a lower bound.

   For an object of known size, the static maximum of a member array's row
   selected by a variable index that may be negative is the size of the
   object when a negative index can keep the pointer inside it, as g.pre in
   front of g.m lets it here, since what's left there depends on the index.
   The dynamic size still chooses the row for an index that isn't negative.  */

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

struct t
{
  char pre[40];
  char m[4][32];
};

char buf[256];
struct t g;

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

__attribute__ ((noipa)) size_t
outer_step_const (struct s *p, int i, int type)
{
  char *q = p->a[i].nm;
  use (q);
  char *r = q + 4;
  use (r);
  if (type == 1)
    return __builtin_dynamic_object_size (r, 1);
  return __builtin_dynamic_object_size (r, 3);
}

__attribute__ ((noipa)) size_t
outer_step_phi (struct s *p, int i, int c)
{
  char *q = c ? p->a[i].nm : p->a[i].nm + 2;
  use (q);
  char *r = q + 4;
  use (r);
  return __builtin_dynamic_object_size (r, 1);
}

__attribute__ ((noipa)) size_t
row (int n, int dynamic)
{
  if (dynamic)
    return __builtin_dynamic_object_size (&g.m[n][3], 1);
  return __builtin_object_size (&g.m[n][3], 1);
}

/* What's really left in BUF from P->a[I].nm[N].  */
#define LEFT(i, n)							\
  (sizeof (buf) - 64 - __builtin_offsetof (struct s, a[0].nm) - 32 * (i) - (n))

/* What's really left in G from g.m[N][K].  */
#define LEFT_G(n, k) (sizeof (g) - 40 - 32 * (n) - (k))

int
main (void)
{
  struct s *p = (struct s *) (buf + 64);

  if (outer (p, 0, 1) != 20 || outer (p, 1, 1) != 20
      || outer (p, -1, 1) < LEFT (-1, 0))
    __builtin_abort ();
  if (outer (p, 0, 3) != 20 || outer (p, 1, 3) != 20
      || outer (p, -1, 3) < 20 || outer (p, -1, 3) > LEFT (-1, 0))
    __builtin_abort ();
  if (outer_step (p, 0, 4) != 16 || outer_step (p, -1, 4) < LEFT (-1, 4)
      || outer_step (p, -1, -8) < LEFT (-1, -8))
    __builtin_abort ();
  if (outer_step_const (p, 0, 1) != 16
      || outer_step_const (p, -1, 1) < LEFT (-1, 4))
    __builtin_abort ();
  if (outer_step_const (p, 0, 3) != 16 || outer_step_const (p, -1, 3) < 16
      || outer_step_const (p, -1, 3) > LEFT (-1, 4))
    __builtin_abort ();
  if (outer_step_phi (p, 0, 1) != 16 || outer_step_phi (p, 0, 0) != 14
      || outer_step_phi (p, -1, 1) < LEFT (-1, 4)
      || outer_step_phi (p, -1, 0) < LEFT (-1, 6))
    __builtin_abort ();
  if (row (1, 0) != sizeof (g) || row (-1, 0) != sizeof (g)
      || row (1, 1) != 32 - 3 || row (-1, 1) < LEFT_G (-1, 3))
    __builtin_abort ();
  return 0;
}
