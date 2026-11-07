/* A variable index that selects a subobject, as N does in &s.m[n][3], takes
   the pointer to a row before the start of the array when it's negative.  A
   maximum size then goes by what's left in the enclosing object, also after
   a further offset.  An index that isn't negative keeps the row, as does
   an index into an object of unknown size, and a minimum size keeps the row
   as a lower bound.  */
/* { dg-do run } */
/* { dg-options "-O2 -Wno-array-bounds" } */

typedef __SIZE_TYPE__ size_t;

struct nt { int n; char m[3][4]; char post[40]; };

struct nt nt[2];

#define NOIPA __attribute__ ((noipa))
#define END ((char *) (nt + 2))

NOIPA size_t
row_d1 (int n)
{
  return __builtin_dynamic_object_size (&nt[1].m[n][3], 1);
}

NOIPA size_t
row_s1 (int n)
{
  return __builtin_object_size (&nt[1].m[n][3], 1);
}

NOIPA size_t
row_d0 (int n)
{
  return __builtin_dynamic_object_size (&nt[1].m[n][3], 0);
}

NOIPA size_t
row_d3 (int n)
{
  return __builtin_dynamic_object_size (&nt[1].m[n][3], 3);
}

NOIPA size_t
row_u_d1 (unsigned n)
{
  return __builtin_dynamic_object_size (&nt[1].m[n][3], 1);
}

NOIPA size_t
p_row_d1 (struct nt *p, int n)
{
  return __builtin_dynamic_object_size (&p->m[n][3], 1);
}

NOIPA size_t
row_back (int n)
{
  char *q = &nt[1].m[n][3];
  return __builtin_dynamic_object_size (q - 2, 1);
}

#define CHECK_LEFT(sz, p, end) \
  do { if ((sz) < (size_t) ((char *) (end) - (char *) (p))) \
	 __builtin_abort (); } while (0)

int
main (void)
{
  CHECK_LEFT (row_d1 (-1), &nt[1].m[-1][3], END);
  CHECK_LEFT (row_s1 (-1), &nt[1].m[-1][3], END);
  CHECK_LEFT (row_d0 (-1), &nt[1].m[-1][3], END);
  CHECK_LEFT (row_back (-1), &nt[1].m[-1][1], END);

  if (row_d1 (1) != 1 || row_d0 (1) != 45 || row_u_d1 (1) != 1
      || row_d3 (1) != 1 || row_d3 (-1) != 1 || row_back (1) != 3
      || p_row_d1 (&nt[1], 1) != 1)
    __builtin_abort ();

  return 0;
}
