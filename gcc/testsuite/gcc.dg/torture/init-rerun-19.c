/* { dg-do run } */
/* { dg-additional-options "-ftrivial-auto-var-init=zero -fzero-init-padding-bits=all" } */

/* An initializer that gives a member a whole value and then overrides part of
   it, which the C front end splits into a store of the whole value and a store
   of the part, still runs again after a jump back to a label above it.  The
   call for the whole value has to run once on each run, a read of the whole
   value through a pointer to the variable has to see the old value, and the
   padding that the whole value brings along has to come out zero.  */

struct m { char a; int b; short c; };
struct s { int hdr; struct m m; long tail; };

static int calls;
static struct s src = { 1, { 2, 3, 4 }, 5 };

__attribute__ ((noipa)) struct m
whole_m (void)
{
  calls++;
  return src.m;
}

/* Every byte of *MP outside of its members must be zero.  */
__attribute__ ((noipa)) void
check_m_padding (const struct m *mp)
{
  const unsigned char *q = (const unsigned char *) mp;
  for (unsigned long i = 0; i < sizeof *mp; i++)
    {
      if (i >= __builtin_offsetof (struct m, a)
	  && i < __builtin_offsetof (struct m, a) + sizeof mp->a)
	continue;
      if (i >= __builtin_offsetof (struct m, b)
	  && i < __builtin_offsetof (struct m, b) + sizeof mp->b)
	continue;
      if (i >= __builtin_offsetof (struct m, c)
	  && i < __builtin_offsetof (struct m, c) + sizeof mp->c)
	continue;
      if (q[i] != 0)
	__builtin_abort ();
    }
}

int
main (void)
{
  struct m dirty;
  __builtin_memset (&dirty, 0xff, sizeof dirty);
  dirty.a = 2;
  dirty.b = 3;
  dirty.c = 4;

  struct s *q = &src;
  int iter = 0;

again:;
  /* One call on each run, rather than one for each member that keeps its
     value or one for the whole loop.  */
  struct s x = { .hdr = 1, .m = whole_m (), .m.b = 10 + iter, .tail = 5 };
  if (calls != iter + 1)
    __builtin_abort ();
  if (x.hdr != 1 || x.m.a != 2 || x.m.b != 10 + iter || x.m.c != 4
      || x.tail != 5)
    __builtin_abort ();

  /* From the second run on, Q points to Y, so the members that keep their
     value have to come from the old value of Y rather than from zeroes.  */
  struct s y = { .hdr = q->hdr, .m = q->m, .m.b = 20 + iter, .tail = q->tail };
  if (y.hdr != 1 || y.m.a != 2 || y.m.b != 20 + iter || y.m.c != 4
      || y.tail != 5)
    __builtin_abort ();
  q = &y;

  /* DIRTY has nonzero padding, none of which may reach Z.  */
  struct s z = { .m = dirty, .m.b = 30 + iter };
  if (z.hdr != 0 || z.m.a != 2 || z.m.b != 30 + iter || z.m.c != 4
      || z.tail != 0)
    __builtin_abort ();
  check_m_padding (&z.m);

  if (++iter < 4)
    goto again;
  return 0;
}
