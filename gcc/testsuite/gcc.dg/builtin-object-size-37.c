/* A constant index below the start of an array that a subobject was taken
   from puts the subobject outside of that array, so the subobject's size
   can't bound what's left from a pointer into it.  The same goes for an
   unsigned index like (size_t) -1 that wraps around to below the array.  An
   index that stays inside the array keeps the subobject's size.  */
/* { dg-do run } */
/* { dg-options "-O2 -Wno-array-bounds" } */

typedef __SIZE_TYPE__ size_t;

struct row { char tag[8]; char m[4][6]; long z; };
struct it { int k; char x[4]; };
struct box { char h[16]; struct it it[3]; };

struct row nt[3];
struct box as[2];

#define bos(p) __builtin_object_size (p, 1)
#define bdos(p) __builtin_dynamic_object_size (p, 1)

/* The size can't be less than what's left of the whole object from P.  */
#define check_left(p, end) \
  do \
    { \
      size_t left = (char *) (end) - (char *) (p); \
      if (bos (p) < left || bdos (p) < left) \
	__builtin_abort (); \
    } \
  while (0)

#define check_member(p, size) \
  do \
    if (bos (p) > (size) || bdos (p) > (size)) \
      __builtin_abort (); \
  while (0)

__attribute__ ((noipa)) void
outer_row (void)
{
  char *p = &nt[1].m[-1][3];
  check_left (p, &nt[3]);
}

__attribute__ ((noipa)) void
outer_row_wrap (void)
{
  char *p = &nt[1].m[(size_t) -1][3];
  check_left (p, &nt[3]);
  char *q = &nt[1].m[(size_t) -1][0];
  check_left (q, &nt[3]);
}

__attribute__ ((noipa)) void
outer_elem (void)
{
  char *p = &as[1].it[-1].x[1];
  check_left (p, &as[2]);
}

__attribute__ ((noipa)) void
outer_param (struct row *e)
{
  char *p = &e->m[-1][3];
  check_left (p, &nt[3]);
}

__attribute__ ((noipa)) void
inner (void)
{
  char *p = &nt[1].m[1][3];
  check_member (p, sizeof nt[1].m[1]);
  char *q = &as[1].it[1].x[1];
  check_member (q, sizeof as[1].it[1].x);
}

int
main (void)
{
  outer_row ();
  outer_row_wrap ();
  outer_elem ();
  outer_param (&nt[1]);
  inner ();
  return 0;
}
