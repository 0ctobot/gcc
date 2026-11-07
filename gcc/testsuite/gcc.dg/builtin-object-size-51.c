/* { dg-do run } */
/* { dg-options "-O2" } */

/* A signed variable step in a loop can take the pointer below the member it
   started in, and so can one of two such steps per iteration even when the
   other brings it back.  The maximum subobject size must cover the whole rest
   of the member from wherever the pointer ends up.  */

typedef __SIZE_TYPE__ size_t;

struct pkt { char tag[16]; char label[24]; long tail; };
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

__attribute__ ((noipa)) void
one (long a, int k)
{
  char *p = L;
  while (k--)
    {
      p += a;
      barrier ();
    }
  CHECK (p);
}

__attribute__ ((noipa)) void
one_int (int a, int k)
{
  char *p = L;
  while (k--)
    {
      p += a;
      barrier ();
    }
  CHECK (p);
}

__attribute__ ((noipa)) void
two (long a, long b, int k)
{
  char *p = L;
  while (k--)
    {
      p += a;
      barrier ();
      p += b;
      barrier ();
    }
  CHECK (p);
}

__attribute__ ((noipa)) void
two_int (int a, int b, int k)
{
  char *p = L;
  while (k--)
    {
      p += a;
      barrier ();
      p += b;
      barrier ();
    }
  CHECK (p);
}

int
main (void)
{
  one (-7, 1);
  one (-3, 2);
  one_int (-5, 1);
  two (-11, 4, 1);
  two (4, -11, 1);
  two (-6, 2, 2);
  two_int (-11, 4, 1);

  if (fails)
    __builtin_abort ();
  return 0;
}
