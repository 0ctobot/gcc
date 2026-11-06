/* PR middle-end/123368 */
/* { dg-do run } */
/* { dg-require-named-sections "" } */

/* A section attribute creates the cgraph node of a nested function before
   the C front end gives the function its DECL_CONTEXT.  Such a function must
   still end up on its parent's nested function list.  Otherwise, it's never
   lowered or emitted, which shows up as an ICE in get_static_chain when
   another nested function causes the parent to be lowered, or else as a link
   failure.  */

#define SEC __attribute__ ((noinline, section (".text.pr123368")))

extern void abort (void);

/* The original reproducer, which crashed in mark_addressable.  */
void
another_nested (void)
{
  void __attribute__ ((section (".text.pr123368"))) g (void) {}
  g ();
  void h (void) {}
}

/* The only nested function in the parent, with an up-level reference.  */
int
only_nested (int a)
{
  int x = a;
  int SEC g (int y) { x += y; return x; }
  int r1 = g (1);
  int r2 = g (2);
  return r1 + r2;
}

/* The only nested function in the parent, with no up-level reference.  */
int
only_nested_no_chain (int a)
{
  int SEC g (int y) { return y + 1; }
  return g (a);
}

/* A sibling that isn't in a named section calls one that is.  */
int
sibling (int a)
{
  int x = a;
  int SEC g (int y) { x += y; return x; }
  int __attribute__ ((noinline)) h (int y)
  {
    int r = g (y);
    return r + x;
  }
  int r1 = h (1);
  int r2 = g (2);
  return r1 + r2;
}

/* The section attribute is on the forward declaration only.  */
int
forward_decl (int a)
{
  int x = a;
  auto int SEC g (int y);
  int __attribute__ ((noinline)) h (int y)
  {
    int r = g (y);
    return r + x;
  }
  int g (int y) { x += y; return x; }
  int r1 = h (1);
  int r2 = g (2);
  return r1 + r2;
}

/* The section attribute is on the definition only.  */
int
forward_decl_2 (int a)
{
  int x = a;
  auto int __attribute__ ((noinline)) g (int y);
  int __attribute__ ((noinline)) h (int y)
  {
    int r = g (y);
    return r + x;
  }
  int __attribute__ ((section (".text.pr123368"))) g (int y)
  {
    x += y;
    return x;
  }
  int r1 = h (1);
  int r2 = g (2);
  return r1 + r2;
}

/* Both levels of nesting are in a named section.  */
int
both_levels (int a)
{
  int x = a;
  int SEC mid (int y)
  {
    int SEC in (int z) { x += z; return x + y; }
    return in (1) + y;
  }
  return mid (2);
}

/* Only one of the levels of nesting is in a named section.  */
int
one_level (int a)
{
  int x = a;
  int __attribute__ ((noinline)) mid (int y)
  {
    int SEC in (int z) { x += z; return x + y; }
    return in (1) + y;
  }
  int SEC mid2 (int y)
  {
    int __attribute__ ((noinline)) in2 (int z) { x += z; return x + y; }
    return in2 (1) + y;
  }
  return mid (2) + mid2 (3);
}

int
main (void)
{
  another_nested ();
  if (only_nested (10) != 11 + 13)
    abort ();
  if (only_nested_no_chain (10) != 11)
    abort ();
  if (sibling (10) != 11 + 11 + 13)
    abort ();
  if (forward_decl (10) != 11 + 11 + 13)
    abort ();
  if (forward_decl_2 (10) != 11 + 11 + 13)
    abort ();
  if (both_levels (10) != 11 + 2 + 2)
    abort ();
  if (one_level (10) != (11 + 2 + 2) + (12 + 3 + 3))
    abort ();
  return 0;
}
