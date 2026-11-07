/* { dg-do run } */
/* { dg-options "-O2 -fwrapv" } */

/* With -fwrapv, a signed sum or product of non-negative values can wrap
   around to a negative one, as can a signed shift to the left with or without
   it.  Such an offset takes the pointer below the member, and a later step
   brings it back, so the maximum subobject size must cover the whole rest of
   the member from there.  */

typedef __SIZE_TYPE__ size_t;

struct pkt { char tag[8]; char label[24]; long tail; };
struct pkt tbl[3];

#define L tbl[1].label
#define barrier() __asm__ __volatile__ ("" : : : "memory")

static int fails;

#define CHECK(p) \
  do \
    { \
      size_t want = L + sizeof (L) - (p); \
      if (__builtin_object_size (p, 1) < want \
	  || __builtin_dynamic_object_size (p, 1) < want) \
	{ \
	  __builtin_printf ("line %d: %zu %zu < %zu\n", __LINE__, \
			    __builtin_object_size (p, 1), \
			    __builtin_dynamic_object_size (p, 1), want); \
	  fails++; \
	} \
    } \
  while (0)

/* X * 3 is -1 for this X.  */
#define MUL_X (__INT_MAX__ / 3 * 2 + 1)

__attribute__ ((noipa)) void
mul_wrap (int n)
{
  int i = (n & __INT_MAX__) * 3;
  char *p = L;
  p += i;
  barrier ();
  p += 1;
  CHECK (p);
}

__attribute__ ((noipa)) void
plus_wrap (int n)
{
  int i = (n & __INT_MAX__) + __INT_MAX__;
  char *p = L;
  p += i;
  barrier ();
  p += 2;
  CHECK (p);
}

__attribute__ ((noipa)) void
shl_wrap (int n, int s)
{
  int i = (n & __INT_MAX__) << s;
  char *p = L;
  p += i;
  barrier ();
  p += 2;
  CHECK (p);
}

__attribute__ ((noipa)) void
aref_mul (int n)
{
  int i = (n & __INT_MAX__) * 3;
  char *p = &L[i];
  CHECK (p);
}

int
main (void)
{
  mul_wrap (MUL_X);
  plus_wrap (__INT_MAX__);
  shl_wrap (__INT_MAX__, 1);
  aref_mul (MUL_X);

  if (fails)
    __builtin_abort ();
  return 0;
}
