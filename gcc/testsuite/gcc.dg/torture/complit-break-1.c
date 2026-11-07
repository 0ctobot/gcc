/* { dg-do run } */
/* { dg-additional-options "-std=gnu89" } */

/* Before C99, the body of a loop or a switch isn't a block of its own, so a
   compound literal there is an object of the enclosing block.  A break or a
   continue in a statement expression in its initializer still belongs to the
   loop or switch around the literal.  Leaving the initializer that way keeps
   the value the literal got last time.  */

struct S { long x; long pad; long a[6]; };

__attribute__ ((noipa)) long
g (long x)
{
  return x;
}

__attribute__ ((noipa)) void
test_break (void)
{
  struct S *p = 0;
  int i;
  for (i = 0; i < 3; i++)
    p = &(struct S) { .x = g (i + 100), .a[3] = ({ if (i == 2) break; 7L; }) };
  if (i != 2 || p->x != 101 || p->a[3] != 7)
    __builtin_abort ();
}

__attribute__ ((noipa)) void
test_continue (void)
{
  struct S *p = 0;
  int i, n = 0;
  for (i = 0; i < 3; i++)
    p = &(struct S) { .x = g (i + 100),
		      .a[3] = ({ if (i == 2) continue; n++; 7L; }) };
  if (i != 3 || n != 2 || p->x != 101 || p->a[3] != 7)
    __builtin_abort ();
}

__attribute__ ((noipa)) void
test_switch (void)
{
  struct S *p = 0;
  int n = 0;
 again:
  switch (n)
    default:
      p = &(struct S) { .x = g (n + 100),
			.a[3] = ({ if (n == 1) break; 7L; }) };
  if (n++ == 0)
    goto again;
  if (p->x != 100 || p->a[3] != 7)
    __builtin_abort ();
}

__attribute__ ((noipa)) void
test_nested (void)
{
  struct S *p = 0;
  long x = 0, a3 = 0;
  int i, k = 0, n = 0;
  while (k < 2)
    {
      for (i = 0; i < 3; i++)
	p = &(struct S) { .x = g (i + 100),
			  .a[3] = ({ if (i == 2) break; n++; 7L; }) };
      x += p->x;
      a3 += p->a[3];
      k++;
    }
  if (k != 2 || n != 4 || x != 202 || a3 != 14)
    __builtin_abort ();
}

int
main (void)
{
  test_break ();
  test_continue ();
  test_switch ();
  test_nested ();
  return 0;
}
