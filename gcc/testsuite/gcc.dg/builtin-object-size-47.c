/* { dg-do run } */
/* { dg-options "-O2" } */

/* A static non-constant offset into a member is presumed to stay in the
   member, so __builtin_object_size (p, 1) is the size of the member when the
   offset is 0, also when the enclosing struct is reached through a parameter
   whose object size is unknown.  The dynamic size knows the offset.

   The rest are controls for the sizes that keep the larger whole size of an
   object reached through a parameter: a dynamic size at a negative run-time
   offset, a constant step back inside the member and mode 0, which has no
   member to presume the pointer stays in.  */

#include "builtin-object-size-common.h"

struct ent { int id; char name[16]; int tail; };

size_t __attribute__ ((noipa))
param_uoff (struct ent *e, unsigned off)
{
  return __builtin_object_size (e->name + off, 1);
}

size_t __attribute__ ((noipa))
param_soff (struct ent *e, int off)
{
  return __builtin_object_size (e->name + off, 1);
}

size_t __attribute__ ((noipa))
param_dyn (struct ent *e, unsigned off)
{
  return __builtin_dynamic_object_size (e->name + off, 1);
}

struct ent * __attribute__ ((noipa))
get (struct ent *e)
{
  return e;
}

size_t __attribute__ ((noipa))
call_uoff (struct ent *e0, unsigned off)
{
  struct ent *e = get (e0);
  return __builtin_object_size (e->name + off, 1);
}

void __attribute__ ((noipa))
use (char *q)
{
  (void) q;
}

/* A negative run-time offset, then a constant step that brings the pointer
   back into the member.  */

size_t __attribute__ ((noipa))
param_sdyn4 (struct ent *e, int off)
{
  char *q = e->name + off;
  q += 4;
  return __builtin_dynamic_object_size (q, 1);
}

size_t __attribute__ ((noipa))
param_sdyn20 (struct ent *e, int off)
{
  char *q = e->name + off;
  q += 20;
  return __builtin_dynamic_object_size (q, 1);
}

/* A constant step back that stays in the member.  */

size_t __attribute__ ((noipa))
param_back (struct ent *e)
{
  char *q = e->name + 8;
  use (q);
  return __builtin_object_size (q - 4, 1);
}

size_t __attribute__ ((noipa))
param_back_dyn (struct ent *e)
{
  char *q = e->name + 8;
  use (q);
  return __builtin_dynamic_object_size (q - 4, 1);
}

size_t __attribute__ ((noipa))
param_back0 (struct ent *e)
{
  char *q = e->name + 8;
  use (q);
  return __builtin_object_size (q - 4, 0);
}

/* Mode 0 of the same pointers knows nothing about the object.  */

size_t __attribute__ ((noipa))
param_uoff0 (struct ent *e, unsigned off)
{
  return __builtin_object_size (e->name + off, 0);
}

size_t __attribute__ ((noipa))
param_soff0 (struct ent *e, int off)
{
  return __builtin_object_size (e->name + off, 0);
}

size_t __attribute__ ((noipa, access (read_write, 1, 2)))
access_soff0 (char *p, size_t n, int off)
{
  return __builtin_object_size (p + off, 0);
}

size_t __attribute__ ((noipa, access (read_write, 1, 2)))
access_sdyn0 (char *p, size_t n, int off)
{
  return __builtin_dynamic_object_size (p + off, 0);
}

int
main (void)
{
  struct ent e;
  char buf[32];

  if (param_uoff (&e, 0) != sizeof (e.name))
    FAIL ();
  if (param_soff (&e, 0) != sizeof (e.name))
    FAIL ();
  if (call_uoff (&e, 0) != sizeof (e.name))
    FAIL ();
  if (param_dyn (&e, 4) != sizeof (e.name) - 4)
    FAIL ();

  if (param_sdyn4 (&e, -4) != sizeof (e.name))
    FAIL ();
  if (param_sdyn20 (&e, -10) != sizeof (e.name) - 10)
    FAIL ();
  if (param_back (&e) != sizeof (e.name) - 4)
    FAIL ();
  if (param_back_dyn (&e) != sizeof (e.name) - 4)
    FAIL ();

  if (param_back0 (&e) != (size_t) -1)
    FAIL ();
  if (param_uoff0 (&e, 0) != (size_t) -1)
    FAIL ();
  if (param_soff0 (&e, 0) != (size_t) -1)
    FAIL ();
  if (access_soff0 (buf + 8, 16, 0) != (size_t) -1)
    FAIL ();
  if (access_sdyn0 (buf + 8, 16, -4) != 20)
    FAIL ();

  DONE ();
}
