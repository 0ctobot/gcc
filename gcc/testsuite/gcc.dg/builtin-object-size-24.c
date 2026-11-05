/* A pointer stepped back from a PHI of member addresses is still in the member
   when the step stays in it for every merged pointer.  A maximum subobject
   size must then come from the member rather than the whole object.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#include "builtin-object-size-common.h"

#define BOS __builtin_object_size
#define DOS __builtin_dynamic_object_size
#define NOIPA __attribute__ ((noipa))

/* A maximum may be larger than the truth, but never smaller.  */
#define CHECK_MAX(got, truth, limit)					\
  do									\
    {									\
      if ((got) < (truth) || (got) > (limit))				\
	FAIL ();							\
    }									\
  while (0)

struct inner
{
  char b[16];
};

struct w
{
  char h[40];
  struct inner m[3];
  char pad[8];
  char a[16];
  char tail[8];
};

struct w g;

/* Steps that stay in the member for both of the pointers merged by a PHI don't
   tell anything about the enclosing object.  */

NOIPA void
test_phi (int c)
{
  char *t = c ? (char *) &g.m[2] : (char *) &g.m[1];
  char *u = c ? t - 8 : t - 16;
  CHECK_MAX (BOS (u, 1), c ? 24 : 48, 48);
  CHECK_MAX (DOS (u, 1), c ? 24 : 48, 48);
}

int
main (void)
{
  test_phi (0);
  test_phi (1);
  DONE ();
}
