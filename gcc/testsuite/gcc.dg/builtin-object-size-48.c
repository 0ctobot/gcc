/* { dg-do run } */
/* { dg-options "-O2" } */

/* An unsigned index zero-extended to an offset can't take a pointer below the
   start of a member, so a constant step forward after it leaves the member's
   size minus the step, or nothing once the step goes past the end of the
   member.  The same holds for a full-width size_t index and for offsets
   computed so that they can't be negative: an int masked down to a small
   value, an unsigned sum and an unsigned value loaded from memory.

   The rest are controls for offsets that can step back, where the size may
   not come out below what's left in the member: a sign-extended int index
   and a subtraction of unsigned values.  */

#include "builtin-object-size-common.h"

struct ent { int id; char name[16]; unsigned tail; };

void __attribute__ ((noipa))
use (void *p)
{
  (void) p;
}

size_t __attribute__ ((noipa))
step4 (unsigned i)
{
  struct ent e;
  use (&e);
  char *q = e.name + i;
  q += 4;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
step20 (unsigned i)
{
  struct ent e;
  use (&e);
  char *q = e.name + i;
  q += 20;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
param_step4 (struct ent *e, unsigned i)
{
  char *q = e->name + i;
  q += 4;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
zstep4 (size_t i)
{
  struct ent e;
  use (&e);
  char *q = e.name + i;
  q += 4;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
zstep20 (size_t i)
{
  struct ent e;
  use (&e);
  char *q = e.name + i;
  q += 20;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
mask_step4 (int i)
{
  struct ent e;
  use (&e);
  char *q = e.name + (i & 7);
  q += 4;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
mask_step20 (int i)
{
  struct ent e;
  use (&e);
  char *q = e.name + (i & 7);
  q += 20;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
plus_step4 (size_t i)
{
  struct ent e;
  use (&e);
  char *q = e.name + (i + 1);
  q += 4;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
plus_step19 (size_t i)
{
  struct ent e;
  use (&e);
  char *q = e.name + (i + 1);
  q += 19;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
load_step4 (size_t *pi)
{
  struct ent e;
  use (&e);
  char *q = e.name + *pi;
  q += 4;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
load_step20 (size_t *pi)
{
  struct ent e;
  use (&e);
  char *q = e.name + *pi;
  q += 20;
  return __builtin_object_size (q, 1);
}

/* A full-width index, then a second, variable step, and the same with each of
   the computed offsets above.  */

struct two { char name[8]; char next[8]; } g;

size_t __attribute__ ((noipa))
size_var (size_t i, size_t j, int c)
{
  char *p = g.name + i;
  if (c)
    p = p + j;
  return __builtin_object_size (p, 1);
}

size_t __attribute__ ((noipa))
mask_var (int i, int j, int c)
{
  char *p = g.name + (i & 7);
  if (c)
    p = p + (j & 7);
  return __builtin_object_size (p, 1);
}

size_t __attribute__ ((noipa))
plus_var (size_t i, size_t j, int c)
{
  char *p = g.name + (i + 1);
  if (c)
    p = p + (j + 1);
  return __builtin_object_size (p, 1);
}

size_t __attribute__ ((noipa))
load_var (size_t *pi, size_t *pj, int c)
{
  char *p = g.name + *pi;
  if (c)
    p = p + *pj;
  return __builtin_object_size (p, 1);
}

size_t __attribute__ ((noipa))
sstep4 (int i)
{
  struct ent e;
  use (&e);
  char *q = e.name + i;
  q += 4;
  return __builtin_object_size (q, 1);
}

/* Start from the member of the second element, so that a step back below it
   stays inside the array.  */

size_t __attribute__ ((noipa))
sstep20 (int i)
{
  struct ent e[2];
  use (e);
  char *q = e[1].name + i;
  q += 20;
  return __builtin_object_size (q, 1);
}

size_t __attribute__ ((noipa))
sub_step4 (size_t i)
{
  struct ent e;
  use (&e);
  char *q = e.name;
  q += i - 8;
  q += 4;
  return __builtin_object_size (q, 1);
}

int
main (void)
{
  struct ent e;
  size_t n = 0;

  if (step4 (0) != sizeof (e.name) - 4)
    FAIL ();
  if (step20 (0) != 0)
    FAIL ();
  if (param_step4 (&e, 0) != sizeof (e.name) - 4)
    FAIL ();
  if (zstep4 (0) != sizeof (e.name) - 4)
    FAIL ();
  if (zstep20 (0) != 0)
    FAIL ();
  if (mask_step4 (0) != sizeof (e.name) - 4)
    FAIL ();
  if (mask_step20 (0) != 0)
    FAIL ();
  if (plus_step4 (0) != sizeof (e.name) - 4)
    FAIL ();
  if (plus_step19 (0) != 0)
    FAIL ();
  if (load_step4 (&n) != sizeof (e.name) - 4)
    FAIL ();
  if (load_step20 (&n) != 0)
    FAIL ();
  if (size_var (1, 1, 1) != sizeof (g.name))
    FAIL ();
  if (mask_var (1, 1, 1) != sizeof (g.name))
    FAIL ();
  if (plus_var (1, 1, 1) != sizeof (g.name))
    FAIL ();
  if (load_var (&n, &n, 1) != sizeof (g.name))
    FAIL ();

  if (sstep4 (-4) < sizeof (e.name))
    FAIL ();
  if (sstep20 (-10) < sizeof (e.name) - 10)
    FAIL ();
  if (sub_step4 (4) < sizeof (e.name))
    FAIL ();

  DONE ();
}
