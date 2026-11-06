/* PR c/123368 */
/* { dg-do run } */
/* { dg-require-effective-target trampolines } */
/* { dg-require-named-sections "" } */

/* Taking the address of a nested function in a named section needs both its
   definition and the parent's frame, which a parent that didn't know about the
   function wouldn't provide.  */

extern void abort (void);

int __attribute__ ((noinline))
call (int (*f) (int), int v)
{
  return f (v);
}

int
outer (int a)
{
  int x = a;
  int __attribute__ ((section (".text.pr123368"))) g (int y)
  {
    x += y;
    return x;
  }
  return call (g, 1) + x;
}

int
main (void)
{
  if (outer (10) != 11 + 11)
    abort ();
  return 0;
}
