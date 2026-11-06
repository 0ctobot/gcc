/* PR middle-end/100529 */
/* { dg-do run } */
/* { dg-additional-options "-std=gnu11 -w" } */

/* A statement-expression arm of a void ?: with its own label reading the
   predicate's temporary through a pointer.  */
extern void abort (void);
extern void *memset (void *, int, __SIZE_TYPE__);

__attribute__ ((noipa)) void
check (const char *p, int n, int v)
{
  volatile char buf[4096];
  for (int i = 0; i < 4096; i++)
    buf[i] = 0x55;
  for (int i = 0; i < n; i++)
    if (p[i] != v)
      abort ();
}

__attribute__ ((noipa)) void
t_goto (int n, int c)
{
  struct S { char a[n]; } x, y;
  memset (&x, 7, n);
  memset (&y, 9, n);
  const char *p;
  (p = (c ? x : y).a)[0]
    ? ({ goto L; L: check (p, n, c ? 7 : 9); (void) 0; })
    : (void) 0;
}

__attribute__ ((noipa)) void
t_case (int n, int c)
{
  struct S { char a[n]; } x, y;
  memset (&x, 0, n);
  memset (&y, 0, n);
  const char *p;
  (p = (c ? x : y).a)[0]
    ? (void) 0
    : ({ switch (c) { case 1: case 0: check (p, n, 0); } (void) 0; });
}

int
main (void)
{
  t_goto (64, 1);
  t_goto (64, 0);
  t_case (64, 1);
  t_case (64, 0);
  return 0;
}
