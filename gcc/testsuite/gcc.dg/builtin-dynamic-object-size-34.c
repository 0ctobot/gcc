/* { dg-do run } */
/* { dg-options "-O2" } */

/* An address with an index that's negative at run time, carried around a
   loop in a PHI, must get the same dynamic size as it does when the object
   size call follows it directly.  The call below runs under a check of the
   index of the current iteration, while the pointer was computed with the
   index of the one before it.  */

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

/* What's really left in BUF from P->a[I].nm.  */
#define LEFT(i) \
  (sizeof (buf) - 64 - __builtin_offsetof (struct s, a[0].nm) - 32 * (i))

__attribute__ ((noipa)) size_t
plain (struct s *p, int i)
{
  return __builtin_dynamic_object_size (&p->a[i].nm, 1);
}

__attribute__ ((noipa)) size_t
carried (struct s *p, const int *idx, int n)
{
  char *q = p->a[1].nm;
  size_t r = 0;

  for (int k = 0; k < n; k++)
    {
      int i = idx[k];
      if (i >= 0)
	r = __builtin_dynamic_object_size (q, 1);
      q = p->a[i].nm;
    }
  return r;
}

int
main (void)
{
  struct s *p = (struct s *) (buf + 64);
  static const int below[] = { -1, 0 };
  static const int inside[] = { 2, 0 };

  /* The reference: the same address sized right where it's computed.  */
  if (plain (p, -1) < LEFT (-1) || plain (p, 2) != 20)
    __builtin_abort ();

  /* The second iteration sizes &p->a[-1].nm.  */
  if (carried (p, below, 2) < LEFT (-1))
    __builtin_abort ();

  /* The second iteration sizes &p->a[2].nm.  A size that can't be worked
     out for the PHI is fine here.  */
  size_t r = carried (p, inside, 2);
  if (r != 20 && r != (size_t) -1)
    __builtin_abort ();

  return 0;
}
