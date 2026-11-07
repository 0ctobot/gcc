/* PR c/100529 */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

/* Each arm of the conditional declares its own variably sized type, so the
   sizes of the result can only come from the arm that runs.  */

extern void abort (void);

__attribute__ ((noipa)) int
gn (void)
{
  return 6;
}

#define MK(v) \
  ({ int m = gn () + (v); struct T { char a[m]; int z; } t; \
     __builtin_memset (t.a, (v), m); t.z = m; t; })

__attribute__ ((noipa)) int
member (int c)
{
  return (c ? MK (1) : MK (5)).z;
}

__attribute__ ((noipa)) unsigned long
in_auto (int c)
{
  __auto_type x = c ? MK (1) : MK (5);
  return sizeof x.a * 10 + x.a[sizeof x.a - 1] + x.z - sizeof x.a;
}

/* An arm can also have a type that's only declared through a comma.  */

__attribute__ ((noipa)) int
in_comma (int c)
{
  return (c ? ((void) gn (), MK (1)) : MK (5)).z;
}

/* An operand can be another such conditional, whose sizes aren't known until
   one of its own operands has run.  */

__attribute__ ((noipa)) int
nested (int c)
{
  return (c > 1 ? MK (1) : c ? MK (3) : MK (5)).z;
}

__attribute__ ((noipa)) unsigned long
nested_size (int c)
{
  return sizeof (c ? (c > 1 ? MK (1) : MK (3)) : MK (5)).a;
}

int
main (void)
{
  if (member (1) != 7 || member (0) != 11)
    abort ();
  if (in_auto (1) != 71 || in_auto (0) != 115)
    abort ();
  if (in_comma (1) != 7 || in_comma (0) != 11)
    abort ();
  if (nested (2) != 7 || nested (1) != 9 || nested (0) != 11)
    abort ();
  if (nested_size (2) != 7 || nested_size (1) != 9 || nested_size (0) != 11)
    abort ();
  return 0;
}
