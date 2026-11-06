/* PR middle-end/100529 */
/* { dg-do run } */
/* { dg-additional-options "-Wno-vla" } */

/* A statement expression's value can get its type from a nested statement
   expression anywhere in the expression that ends it, not just right at the
   top, so all of that has to run before the temporary can be allocated.  */

__attribute__ ((noipa)) int
get_size (void)
{
  return 64;
}

static char buf[64] = { [5] = 3 };

__attribute__ ((noipa)) void
fill (char *p, int c, int n)
{
  for (int i = 0; i < n; i++)
    p[i] = c;
}

__attribute__ ((noipa)) int
comma (void)
{
  return ({ int k = 2; (k += 1, ({ int m = get_size (); struct { char a[m]; } t; fill (t.a, k, m); t; })); }).a[5];
}

__attribute__ ((noipa)) int
member (void)
{
  return ({ int k = 3; ({ int m = get_size (); struct { struct { char a[m]; } in; } t; fill ((char *) &t, k, m); t; }).in; }).a[5];
}

__attribute__ ((noipa)) int
deref (void)
{
  return ({ int k = 3; (void) k; *({ int m = get_size (); struct { char a[m]; } *p = (void *) buf; p; }); }).a[5];
}

int
main (void)
{
  if (comma () != 3)
    __builtin_abort ();
  if (member () != 3)
    __builtin_abort ();
  if (deref () != 3)
    __builtin_abort ();
  return 0;
}
