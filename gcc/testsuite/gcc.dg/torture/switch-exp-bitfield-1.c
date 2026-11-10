/* { dg-do run } */

/* Switch conversion's exponential index transform on a bit-field index
   computes .FFS in int.  The result must not get the bit-field's type,
   since FFS of an int can be 32, which doesn't fit a narrow field, and a
   wrapped range for it made every case below take the default.  */

#define F(N, T, W)							\
  struct s##N { unsigned a : 3; T f : W; };				\
  __attribute__ ((noipa)) int						\
  g##N (const struct s##N *p)						\
  {									\
    switch (p->f)							\
      {									\
      case 1: return 177;						\
      case 2: return 267;						\
      case 4: return 54;						\
      default: return 251;						\
      }									\
  }

F (3, int, 3) F (4, int, 4) F (5, int, 5) F (6, int, 6) F (7, int, 7)
F (8, int, 8) F (13, unsigned, 3) F (14, unsigned, 4) F (15, unsigned, 5)
F (16, unsigned, 6) F (24, long, 4) F (25, signed char, 5) F (26, short, 4)

struct sl { unsigned a : 3; int f : 5; };

static inline int
kind (const struct sl *p)
{
  return p->f;
}

/* A local copy of the field.  */
__attribute__ ((noipa)) int
l1 (const struct sl *p)
{
  int v = p->f;

  switch (v)
    {
    case 1: return 177;
    case 2: return 267;
    case 4: return 54;
    default: return 251;
    }
}

/* An inline accessor, with a fourth case.  */
__attribute__ ((noipa)) int
l2 (const struct sl *p)
{
  switch (kind (p))
    {
    case 1: return 177;
    case 2: return 267;
    case 4: return 54;
    case 8: return 9;
    default: return 251;
    }
}

static int
ref (long v)
{
  switch (v)
    {
    case 1: return 177;
    case 2: return 267;
    case 4: return 54;
    case 8: return 9;
    default: return 251;
    }
}

#define T(N)					\
  do						\
    {						\
      struct s##N s = { 0 };			\
      s.f = v;					\
      if (g##N (&s) != (s.f == 8 ? 251 : ref (s.f)))	\
	__builtin_abort ();			\
    }						\
  while (0)

int
main (void)
{
  for (int v = -64; v < 64; v++)
    {
      struct sl s = { 0 };

      T (3); T (4); T (5); T (6); T (7); T (8); T (13); T (14); T (15);
      T (16); T (24); T (25); T (26);

      s.f = v;
      if (l1 (&s) != (s.f == 8 ? 251 : ref (s.f)) || l2 (&s) != ref (s.f))
	__builtin_abort ();
    }
  return 0;
}
