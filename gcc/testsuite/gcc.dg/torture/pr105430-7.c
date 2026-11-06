/* PR c/105430 */
/* Designators overriding parts of a whole compound literal whose type is a
   compatible struct of the same tag from another scope.  The literal's elements
   name the fields of the other type, which the override has to replace or keep
   just like those of the member's own type.  */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

struct K2 { short p; short q; };
struct K3 { short p; short q; short r; };
struct I { short a; short b; };
struct N { struct I i; short c; };
struct E { };
struct M { struct E d; int x; struct E e; int y; };
typedef struct K2 OT2;
typedef struct K3 OT3;
typedef struct N ON;
typedef struct M OM;

__attribute__((noipa)) short
h (short x)
{
  return x;
}

__attribute__((noipa)) struct E
fe (int *c)
{
  struct E e;
  ++*c;
  return e;
}

/* The fields that collide are the last of both chains.  */
static void
override_last_of_two (void)
{
  struct K2 { short p; short q; };
  static struct W { struct K2 s; } w = { .s = (OT2) { 4, 5 }, .s.q = 9 };
  if (w.s.p != 4 || w.s.q != 9)
    __builtin_abort ();
}

static void
override_middle (void)
{
  struct K3 { short p; short q; short r; };
  static struct W { struct K3 s; } w = { .s = (OT3) { 4, 5, 6 }, .s.q = 9 };
  if (w.s.p != 4 || w.s.q != 9 || w.s.r != 6)
    __builtin_abort ();
}

static void
override_last_of_three (void)
{
  struct K3 { short p; short q; short r; };
  static struct W { struct K3 s; } w = { .s = (OT3) { 4, 5, 6 }, .s.r = 9 };
  if (w.s.p != 4 || w.s.q != 5 || w.s.r != 9)
    __builtin_abort ();
}

static void
override_twice (void)
{
  struct K3 { short p; short q; short r; };
  static struct W { struct K3 s; } w
    = { .s = (OT3) { 4, 5, 6 }, .s.r = 9, .s.p = 7 };
  if (w.s.p != 7 || w.s.q != 5 || w.s.r != 9)
    __builtin_abort ();
}

/* The other type's fields also appear one level further in.  */
static void
override_nested (void)
{
  struct I { short a; short b; };
  struct N { struct I i; short c; };
  static struct W { struct N s; } w
    = { .s = (ON) { { 1, 2 }, 3 }, .s.i.b = 9 };
  if (w.s.i.a != 1 || w.s.i.b != 9 || w.s.c != 3)
    __builtin_abort ();
}

static void
override_nonconstant (void)
{
  struct K2 { short p; short q; };
  struct W { struct K2 s; } w = { .s = (OT2) { h (4), h (5) }, .s.q = h (9) };
  if (w.s.p != 4 || w.s.q != 9)
    __builtin_abort ();
}

/* Empty members share a position and only the chain can order them.  */
static void
override_empty (void)
{
  int c = 0;
  struct M { struct E d; int x; struct E e; int y; };
  struct W { struct M s; } w
    = { .s = (OM) { { }, h (1), { }, h (2) }, .s.e = fe (&c), .s.d = fe (&c) };
  if (w.s.x != 1 || w.s.y != 2 || c != 2)
    __builtin_abort ();
}

int
main (void)
{
  override_last_of_two ();
  override_middle ();
  override_last_of_three ();
  override_twice ();
  override_nested ();
  override_nonconstant ();
  override_empty ();
  return 0;
}
