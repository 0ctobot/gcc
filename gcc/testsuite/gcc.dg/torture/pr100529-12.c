/* PR c/100529 */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

/* Each arm of a conditional declares its own variably sized struct type of
   the same tag, so the sizes of the result can only come from the arm that
   runs.  That doesn't depend on anything else in the function, like a loop, a
   label or a call that can return twice, and an arm can change n before it
   declares its type.  */

extern void abort (void);

int *p;

__attribute__ ((noipa)) void
use (int *q)
{
  p = q;
}

#define ARM(x) \
  ({ x; struct T { char a[n]; } t; __builtin_memset (&t, 7, sizeof t); t; })

__attribute__ ((noipa)) int
straight (int n, int c)
{
  int r = (c ? ARM (if (p) *p = 6) : ARM ()).a[0];
  use (&n);
  return r;
}

__attribute__ ((noipa)) int
loop_after (int n, int c)
{
  int r = (c ? ARM (if (p) *p = 6) : ARM ()).a[0];
  use (&n);
  for (int i = 0; i < 2; i++)
    use (0);
  return r;
}

__attribute__ ((noipa)) int
loop_before (int n, int c)
{
  for (int i = 0; i < 2; i++)
    use (0);
  int r = (c ? ARM (if (p) *p = 6) : ARM ()).a[0];
  use (&n);
  return r;
}

__attribute__ ((noipa)) int
label (int n, int c)
{
  int r = (c ? ARM (if (p) *p = 6) : ARM ()).a[0];
  use (&n);
 unused_label: __attribute__ ((unused));
  return r;
}

__attribute__ ((noipa)) int
setjmp_after (int n, int c)
{
  static void *buf[5];
  int r = (c ? ARM (if (p) *p = 6) : ARM ()).a[0];
  use (&n);
  if (__builtin_setjmp (buf))
    abort ();
  return r;
}

/* The arm that runs stores 6 to n before it declares its type, and the size of
   the result has to follow.  */

__attribute__ ((noipa)) unsigned long
in_loop (int n, int c)
{
  unsigned long s = 0;
  for (int i = 0; i < 2; i++)
    {
      s = sizeof (c ? ARM (if (p) *p = 6) : ARM ());
      use (&n);
    }
  use (0);
  return s;
}

/* A constant size struct with a pointer to an array sized in each arm.  */

int arr[3];

#define PTR(v) ({ int m = 3; struct S { int (*q)[m]; } v = { &arr }; v; })

__attribute__ ((noipa)) unsigned long
pointer_size (int c)
{
  return sizeof *(c ? PTR (x) : PTR (y)).q;
}

int
main (void)
{
  if (straight (5, 1) != 7 || straight (5, 0) != 7)
    abort ();
  use (0);
  if (loop_after (5, 1) != 7 || loop_after (5, 0) != 7)
    abort ();
  use (0);
  if (loop_before (5, 1) != 7 || loop_before (5, 0) != 7)
    abort ();
  use (0);
  if (label (5, 1) != 7 || label (5, 0) != 7)
    abort ();
  use (0);
  if (setjmp_after (5, 1) != 7 || setjmp_after (5, 0) != 7)
    abort ();
  use (0);
  if (in_loop (5, 1) != 6 || in_loop (5, 0) != 5)
    abort ();
  if (pointer_size (1) != 3 * sizeof (int)
      || pointer_size (0) != 3 * sizeof (int))
    abort ();
  return 0;
}
