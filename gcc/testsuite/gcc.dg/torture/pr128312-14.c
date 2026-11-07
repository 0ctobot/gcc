/* PR tree-optimization/128312 */
/* { dg-do run { target { { i?86-*-* x86_64-*-* } && lp64 } } } */

/* The ms_abi va_list is a plain pointer to the next argument, so a va_arg in
   a second run of an initializer reads the old value of the variable when the
   va_list points into it.  */

struct S { long x; long a[8]; };

static long *volatile gp;

int
main (void)
{
  int n = 0;
  __builtin_ms_va_list ap;
 again:
  ap = (char *) gp;
  struct S s = { .x = 1, .a[3] = n ? __builtin_va_arg (ap, long) + 1 : 7 };
  gp = &s.a[3];
  if (n++ == 0)
    goto again;
  if (s.a[3] != 8)
    __builtin_abort ();
  return 0;
}
