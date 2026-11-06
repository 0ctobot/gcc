/* The static maximum for a subobject presumes that a pointer stepped back in a
   loop or moved by a non-constant offset stayed where it started.  A constant
   going forward from there can still land inside the subobject, so it mustn't
   report 0 for going past the end of that maximum.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#include "builtin-object-size-common.h"

#define BOS(p, t) __builtin_object_size (p, t)
#define DOS(p, t) __builtin_dynamic_object_size (p, t)
#define BARRIER() __asm__ ("" : : : "memory")

struct E
{
  int id;
  char name[16];
};

struct E arr[4];
char buf[16];
volatile int count;
volatile long off;

/* The target of each pointer has 2 bytes left in the member.  */

static void __attribute__ ((noinline))
test_loop (int k)
{
  char *p = arr[1].name + 2;

  for (int j = 0; j < k; j++)
    p += -1;
  p += 16;
  if (p != arr[1].name + 14)
    FAIL ();
  if (BOS (p, 1) < 2)
    FAIL ();
  if (DOS (p, 1) < 2)
    FAIL ();
}

static void __attribute__ ((noinline))
test_variable (long n)
{
  char *p = arr[1].name;

  p += n;
  BARRIER ();
  p += 16;
  if (p != arr[1].name + 14)
    FAIL ();
  if (BOS (p, 1) < 2)
    FAIL ();
  if (DOS (p, 1) < 2)
    FAIL ();
}

/* The target has 6 bytes left in the member, and the forward constant is on the
   loop itself.  */

static void __attribute__ ((noinline))
test_cycle (int k)
{
  char *p = arr[1].name + 2;

  for (int j = 0; j < k; j++)
    {
      p += -8;
      BARRIER ();
      p += 16;
      if (BOS (p, 1) < 6)
	FAIL ();
    }
  if (p != arr[1].name + 10)
    FAIL ();
}

/* The checks stay exact for everything that doesn't need the above.  */

static void __attribute__ ((noinline))
test_kept (int k, long n)
{
  char *p = arr[1].name;

  /* Only ever going forward can't come back into the member.  */
  for (int j = 0; j < k; j++)
    p += 1;
  p += 16;
  if (BOS (p, 1) != 0)
    FAIL ();

  /* Staying inside the maximum keeps the size of the member.  */
  p = arr[1].name + 2;
  for (int j = 0; j < k; j++)
    p += -1;
  p += 8;
  if (BOS (p, 1) != 8)
    FAIL ();

  p = arr[1].name;
  p += n;
  BARRIER ();
  p += 4;
  if (BOS (p, 1) != 12)
    FAIL ();

  /* There's nothing around a whole object to go to.  */
  p = buf;
  p += n;
  BARRIER ();
  p += 16;
  if (BOS (p, 1) != 0)
    FAIL ();
}

int
main (void)
{
  count = 4;
  off = -2;
  test_loop (count);
  test_variable (off);
  count = 1;
  test_cycle (count);
  count = 2;
  off = 2;
  test_kept (count, off);
  DONE ();
}
