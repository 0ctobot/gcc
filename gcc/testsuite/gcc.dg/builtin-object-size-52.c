/* { dg-do run } */
/* { dg-options "-O2" } */

/* The result that __builtin_add_overflow and __builtin_mul_overflow store is
   the true result wrapped around to its type, so a signed one can be negative
   when neither argument is.  GCC defines a signed shift to the left the same
   way.  Such an offset takes the pointer below the member, and a later step
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
mulovf (int n)
{
  int i;
  __builtin_mul_overflow (n & __INT_MAX__, 3, &i);
  char *p = L;
  p += i;
  barrier ();
  p += 1;
  CHECK (p);
}

__attribute__ ((noipa)) void
addovf (int n)
{
  int i;
  __builtin_add_overflow (n & __INT_MAX__, __INT_MAX__, &i);
  char *p = L;
  p += i;
  barrier ();
  p += 2;
  CHECK (p);
}

__attribute__ ((noipa)) void
shl (int n, int s)
{
  int i = (n & __INT_MAX__) << s;
  char *p = L;
  p += i;
  barrier ();
  p += 2;
  CHECK (p);
}

int
main (void)
{
  mulovf (MUL_X);
  addovf (__INT_MAX__);
  shl (__INT_MAX__, 1);

  if (fails)
    __builtin_abort ();
  return 0;
}
