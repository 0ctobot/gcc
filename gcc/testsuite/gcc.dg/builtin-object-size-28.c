/* A pointer that may have gone below a member can come back into it, and the
   size of the member says nothing about how much is left for it then.  A valid
   copy into the member must not abort.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#include "builtin-object-size-common.h"

#define BOS __builtin_object_size
#define DOS __builtin_dynamic_object_size
#define BARRIER() __asm__ __volatile__ ("" : : : "memory")

struct entry
{
  char hdr[8];
  char name[16];
  long pad;
};

struct entry arr[4];
char src[64];
volatile int four = 4;

/* The target of each pointer has 12 bytes left in the member.  */

#define CHECK(p, left)							\
  do									\
    {									\
      if ((p) != arr[1].name + 16 - (left))				\
	FAIL ();							\
      if (BOS (p, 1) < (left) || DOS (p, 1) < (left))			\
	FAIL ();							\
      __builtin___memcpy_chk (p, src, (left), BOS (p, 1));		\
    }									\
  while (0)

static void __attribute__ ((noinline))
test_loop (int k)
{
  char *p = arr[1].name + 2;

  for (int j = 0; j < k; j++)
    p += -1;
  p += 6;
  CHECK (p, 12);
}

static void __attribute__ ((noinline))
test_negated (long v)
{
  char *p = arr[1].name + 2 - v;

  BARRIER ();
  p += 6;
  CHECK (p, 12);
}

static void __attribute__ ((noinline))
test_variable (long n)
{
  char *p = arr[1].name;

  p += n;
  BARRIER ();
  p += 6;
  CHECK (p, 12);
}

/* The member is left behind for good here, so the copy spans several members
   and only the whole object bounds it.  */

static void __attribute__ ((noinline))
test_below (long v)
{
  char *p = arr[1].name - v;

  if (BOS (p, 1) < 16 + v)
    FAIL ();
  __builtin___memcpy_chk (p, src, 16 + v, BOS (p, 1));
}

/* Pointers that only go forward or follow exact constants keep the checks for
   the member.  */

static void __attribute__ ((noinline))
test_kept (int k)
{
  char *p = arr[1].name;

  for (int j = 0; j < k; j++)
    p += 1;
  p += 4;
  if (BOS (p, 1) != 12)
    FAIL ();

  p = arr[1].name + 2;
  BARRIER ();
  p += -1;
  BARRIER ();
  p += 3;
  if (BOS (p, 1) != 12)
    FAIL ();
}

int
main (void)
{
  test_loop (four);
  test_negated (four);
  test_variable (four - 6);
  test_below (four);
  test_kept (four);
  DONE ();
}
