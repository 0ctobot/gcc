/* { dg-do run } */
/* { dg-options "-O2" } */

/* An offset that isn't constant, or that repeats in a loop, and lands at or
   past the end of a member must give the member's dynamic size of 0 when the
   step that adds it is written in the function itself, whether the enclosing
   object is known or not and whether the offset is signed or not.  Such steps
   that follow one another count as one offset: a pointer that one of them
   takes below the member and a later one brings back into it or past its end
   gets the exact size or 0.  A pointer that lands below the member keeps a
   size that covers what's left.  An offset added in an inlined function can't
   be told from one that folding combined, so it doesn't get a size of 0.  */

typedef __SIZE_TYPE__ size_t;

struct ent { int id; char name[16]; int tail[8]; };
struct grp { int a; struct { int x; char y[12]; } g; char z[8]; };
struct ent ge;
struct grp gg;

__attribute__ ((noipa)) size_t
param_off (struct ent *e, unsigned off)
{
  return __builtin_dynamic_object_size (e->name + off, 1);
}

__attribute__ ((noipa)) size_t
global_off (unsigned long off)
{
  return __builtin_dynamic_object_size (gg.g.y + off, 1);
}

static inline __attribute__ ((always_inline)) size_t
member_size (void *p)
{
  return __builtin_dynamic_object_size (p, 1);
}

__attribute__ ((noipa)) size_t
caller_off (struct ent *e, unsigned off)
{
  return member_size (e->name + off);
}

static inline __attribute__ ((always_inline)) size_t
member_size_at (char *p, unsigned long off)
{
  return __builtin_dynamic_object_size (p + off, 1);
}

__attribute__ ((noipa)) size_t
inlined_off (unsigned long off)
{
  return member_size_at (ge.name, off);
}

__attribute__ ((noipa)) size_t
signed_off (struct ent *e, long off)
{
  return __builtin_dynamic_object_size (e->name + off, 1);
}

__attribute__ ((noipa)) size_t
loop_off (struct ent *e, int n)
{
  char *p = e->name;
  size_t sz = 16;
  for (int k = 0; k < n; k++)
    {
      p += 2;
      sz = __builtin_dynamic_object_size (p, 1);
    }
  return sz;
}

__attribute__ ((noipa)) size_t
back_off (struct ent *e, int i, int j)
{
  char *q = e->name + i;
  q += j;
  return __builtin_dynamic_object_size (q, 1);
}

__attribute__ ((noipa)) size_t
then_past (struct ent *e, int i)
{
  char *q = e->name;
  q += i;
  q += 20;
  return __builtin_dynamic_object_size (q, 1);
}

__attribute__ ((noipa)) size_t
then_in (struct ent *e, int i)
{
  char *q = e->name;
  q += i;
  q += 4;
  return __builtin_dynamic_object_size (q, 1);
}

__attribute__ ((noipa)) size_t
then_back (struct ent *e, long i)
{
  char *q = e->name + i;
  return __builtin_dynamic_object_size (q - 4, 1);
}

__attribute__ ((noipa)) size_t
unsigned_back (struct ent *e, unsigned i)
{
  char *q = e->name;
  q -= 8;
  q += i;
  q += 4;
  return __builtin_dynamic_object_size (q, 1);
}

__attribute__ ((noipa)) size_t
loop_then (struct ent *e, int n, int i)
{
  char *q = e->name;
  for (int k = 0; k < n; k++)
    q += 2;
  q += i;
  q += 4;
  return __builtin_dynamic_object_size (q, 1);
}

__attribute__ ((noipa)) size_t
inlined_then (struct ent *e, int i)
{
  return member_size_at (e->name + i, 20);
}

__attribute__ ((noipa)) size_t
const_off (struct ent *e)
{
  return __builtin_dynamic_object_size (e->name + 4, 1);
}

int
main (void)
{
  struct ent e;

  if (param_off (&e, 0) != 16 || param_off (&e, 15) != 1
      || param_off (&e, 16) != 0 || param_off (&e, 20) != 0)
    __builtin_abort ();

  if (global_off (11) != 1 || global_off (12) != 0 || global_off (13) != 0)
    __builtin_abort ();

  if (caller_off (&e, 4) != 12 || caller_off (&e, 20) != 0)
    __builtin_abort ();

  if (inlined_off (4) != 12 || inlined_off (20) < 16)
    __builtin_abort ();

  if (signed_off (&e, 5) != 11 || signed_off (&e, 20) != 0
      || signed_off (&e, -4) < 20)
    __builtin_abort ();

  if (loop_off (&e, 3) != 10 || loop_off (&e, 9) != 0)
    __builtin_abort ();

  if (back_off (&e, 20, -8) != 4 || back_off (&e, -4, 8) != 12
      || back_off (&e, -4, 2) < 18)
    __builtin_abort ();

  if (then_past (&e, 0) != 0 || then_past (&e, -4) != 0
      || then_past (&e, 4) != 0 || then_past (&e, 16) != 0
      || then_past (&e, -16) != 12 || then_past (&e, -20) != 16)
    __builtin_abort ();

  if (then_in (&e, 0) != 12 || then_in (&e, -4) != 16
      || then_in (&e, 4) != 8 || then_in (&e, 12) != 0
      || then_in (&e, 16) != 0 || then_in (&e, -6) < 18)
    __builtin_abort ();

  if (then_back (&e, 24) != 0 || then_back (&e, 20) != 0
      || then_back (&e, 8) != 12 || then_back (&e, 2) < 18)
    __builtin_abort ();

  if (unsigned_back (&e, 4) != 16 || unsigned_back (&e, 8) != 12
      || unsigned_back (&e, 20) != 0 || unsigned_back (&e, 2) < 18)
    __builtin_abort ();

  if (loop_then (&e, 2, 8) != 0 || loop_then (&e, 2, -4) != 12
      || loop_then (&e, 0, -4) != 16 || loop_then (&e, 1, -8) < 18)
    __builtin_abort ();

  if (inlined_then (&e, 0) == 0 || inlined_then (&e, 4) == 0)
    __builtin_abort ();

  if (const_off (&e) != 12)
    __builtin_abort ();

  return 0;
}
