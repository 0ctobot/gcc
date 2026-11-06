/* { dg-do run } */
/* { dg-options "-O2" } */

/* A pointer that leaves its subobject while the enclosing object is unknown
   can get an internal size of offset_limit or more, which lets a later step
   back into the subobject recover the subobject's size.  Such a size must
   never reach the user, who gets (size_t) -1 for it instead.  A pointer
   that's back inside gets the remaining size of the subobject, or (size_t) -1
   where the step back goes through a PHI.  A real size of offset_limit or
   more, like the one from an allocation of (size_t) -5 bytes, is returned
   unchanged.  */

typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;

#define EQ(expr, want) if ((expr) != (want)) __builtin_abort ()

/* EXPR is either WANT or the unknown size.  */
#define EQ_OR_UNKNOWN(expr, want) \
  EQ ((expr) == (size_t) -1 ? (size_t) (want) : (expr), want)

struct t { int a; char m[12]; int z; } w[3];

#define MSZ sizeof (w[0].m)
#define TSZ sizeof (struct t)

static char big[64];

__attribute__ ((noipa)) void *
get (size_t k)
{
  return k > sizeof (big) ? 0 : big;
}

__attribute__ ((noipa, alloc_size (1))) void *
myalloc (size_t k)
{
  return get (k);
}

/* An array index that's negative at run time.  */

__attribute__ ((noipa)) size_t
elt (struct t *p, ptrdiff_t i)
{
  return __builtin_dynamic_object_size (&p->m[i], 1);
}

/* A pointer stepped back from the member, then forward again.  */

__attribute__ ((noipa)) size_t
plain (struct t *p, ptrdiff_t n)
{
  char *q = p->m - n;
  return __builtin_dynamic_object_size (q, 1);
}

__attribute__ ((noipa)) size_t
fwd (struct t *p, ptrdiff_t n)
{
  char *q = p->m - n;
  q += 4;
  return __builtin_dynamic_object_size (q, 1);
}

__attribute__ ((noipa)) size_t
phi_fwd (struct t *p, ptrdiff_t n, int c)
{
  char *q = p->m - n;
  if (c)
    q = p->m;
  q += 4;
  return __builtin_dynamic_object_size (q, 1);
}

__attribute__ ((noipa)) size_t
loop_fwd (struct t *p, ptrdiff_t n, int k)
{
  char *q = p->m - n;
  for (int j = 0; j < k; j++)
    q += 2;
  return __builtin_dynamic_object_size (q, 1);
}

/* A PHI of a pointer that may be outside its subobject and a pointer with
   a real size from an allocation.  */

__attribute__ ((noipa)) size_t
mixed (struct t *p, ptrdiff_t n, size_t k, int c)
{
  char *q = c ? p->m - n : (char *) myalloc (k);
  return __builtin_dynamic_object_size (q, 1);
}

int
main (void)
{
  struct t *p = &w[1];
  size_t big_k = (size_t) __PTRDIFF_MAX__ + 3;

  EQ (elt (p, -2), (size_t) -1);
  EQ (elt (p, 0), MSZ);
  EQ (elt (p, 3), MSZ - 3);

  /* Before the member.  */
  EQ (plain (p, 2), (size_t) -1);
  EQ (fwd (p, 8), (size_t) -1);
  EQ (phi_fwd (p, 8, 0), (size_t) -1);
  EQ (loop_fwd (p, 8, 3), (size_t) -1);
  EQ (mixed (p, 2, 8, 1), (size_t) -1);
  EQ (mixed (p, TSZ, 8, 1), (size_t) -1);

  /* Past the end of the member.  */
  EQ (plain (p, -(ptrdiff_t) TSZ), 0);
  EQ (fwd (p, -(ptrdiff_t) MSZ), 0);
  EQ (phi_fwd (p, -(ptrdiff_t) TSZ, 0), (size_t) -1);
  EQ (loop_fwd (p, -9, 3), (size_t) -1);
  EQ (mixed (p, -(ptrdiff_t) TSZ, 8, 1), (size_t) -1);

  /* Back inside the member.  */
  EQ (plain (p, 0), MSZ);
  EQ (plain (p, -9), MSZ - 9);
  EQ (fwd (p, 2), MSZ - 2);
  EQ_OR_UNKNOWN (phi_fwd (p, 2, 0), MSZ - 2);
  EQ (phi_fwd (p, 8, 1), MSZ - 4);
  EQ_OR_UNKNOWN (loop_fwd (p, 2, 3), MSZ - 4);
  EQ (loop_fwd (p, -9, 1), MSZ - 11);
  EQ (mixed (p, -4, 8, 1), MSZ - 4);

  /* Real sizes from the allocation.  */
  EQ (mixed (p, 0, 8, 0), 8);
  EQ (mixed (p, 0, (size_t) -5, 0), (size_t) -5);
  EQ (mixed (p, 0, big_k, 0), big_k);

  return 0;
}
