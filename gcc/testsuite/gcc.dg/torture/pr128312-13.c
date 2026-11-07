/* PR middle-end/128312 */
/* { dg-do run } */
/* { dg-additional-options "-std=gnu89" } */

/* Before C99, a loop and its body aren't blocks of their own, so a compound
   literal in a loop body without braces is one object of the enclosing block
   that gets initialized again on every iteration.  */

struct S { int x; int pad; long a[6]; };

__attribute__ ((noipa)) long
g (long x)
{
  return x;
}

int
main (void)
{
  struct S *p = 0;
  int i;
  for (i = 0; i < 3; i++)
    p = &(struct S) { .x = p ? p->x + 1 : 5, .a[3] = g (p ? p->x : -1) };
  if (p->x != 7 || p->a[3] != 6)
    __builtin_abort ();
  return 0;
}
