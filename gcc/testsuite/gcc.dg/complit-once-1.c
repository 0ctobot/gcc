/* One evaluation of a compound literal must evaluate its initializer once.
   The front end reuses one literal for both the read and the write of a
   compound assignment, and the read can fold down to the bare literal, the left
   operand of a comma expression or the operand of a SAVE_EXPR.  */
/* { dg-do run } */
/* { dg-options "-std=gnu23 -O0" } */

extern void abort (void);

static int calls;
static int arr[2];

__attribute__ ((noipa)) static int
val (int x)
{
  calls++;
  return x;
}

__attribute__ ((noipa)) static int *
pval (int *p)
{
  calls++;
  return p;
}

__attribute__ ((noipa)) static _Complex int
cval (int re, int im)
{
  calls++;
  return re + im * 1i;
}

__attribute__ ((noipa)) static int
loop_identity (int n)
{
  int sum = 0;
  for (int i = 0; i < n; i++)
    sum += (int) { val (i) } += 0;
  return sum;
}

#define CHECK(expr, want, wcalls)		\
  do						\
    {						\
      calls = 0;				\
      if ((expr) != (want) || calls != (wcalls))	\
	abort ();				\
    }						\
  while (0)

int
main (void)
{
  /* The operation is an identity, leaving the literal assigned to itself.  */
  CHECK ((int) { val (1) } += 0, 1, 1);
  CHECK ((int) { val (1) } -= 0, 1, 1);
  CHECK ((int) { val (2) } *= 1, 2, 1);
  CHECK ((int) { val (2) } /= 1, 2, 1);
  CHECK ((int) { val (2) } |= 0, 2, 1);
  CHECK ((int) { val (2) } ^= 0, 2, 1);
  CHECK ((int) { val (2) } <<= 0, 2, 1);
  CHECK ((int) { val (2) } >>= 0, 2, 1);
  CHECK ((long) { val (2) } += 0, 2, 1);
  CHECK ((char) { val (2) } += 0, 2, 1);
  CHECK ((register int) { val (2) } += 0, 2, 1);
  CHECK ((volatile int) { val (2) } += 0, 2, 1);
  CHECK ((bool) { val (1) } |= 0, 1, 1);
  CHECK ((int *) { pval (arr) } += 0, arr, 1);
  CHECK ((int *) { pval (arr) } -= 0, arr, 1);
  CHECK (__real__ ((_Complex int) { cval (1, 5) } += 0), 1, 1);
  CHECK (__imag__ ((_Complex int) { cval (1, 5) } *= 1), 5, 1);

  /* The same, next to other literals and evaluations.  */
  CHECK ((int) { (int) { val (1) } } += 0, 1, 1);
  CHECK ((int) { (int) { val (1) } += 0 } += 0, 1, 1);
  CHECK (val ((int) { val (1) } += 0), 1, 2);
  CHECK (val (1) ? (int) { val (1) } += 0 : 0, 1, 2);
  CHECK (((int) { val (1) } += 0, 7), 7, 1);
  CHECK (({ int y = (int) { val (1) } += 0; y; }), 1, 1);
  CHECK (({ (int) { val (1) } += 0; }), 1, 1);
  CHECK (({ int v[(int) { val (2) } += 0]; (long) sizeof (v); }),
	 2 * sizeof (int), 1);

  /* The operation discards the value, leaving a comma expression.  */
  CHECK ((int) { val (2) } *= 0, 0, 1);
  CHECK ((int) { val (2) } &= 0, 0, 1);
  CHECK ((bool) { val (0) } |= 1, 1, 1);

  /* -fsanitize=shift and the like save the operands of these operations.  */
  CHECK ((int) { val (1) } <<= 1, 2, 1);
  CHECK ((int) { val (6) } >>= 1, 3, 1);
  CHECK ((int) { val (6) } /= 3, 2, 1);
  CHECK ((int) { val (7) } %= 3, 1, 1);

  /* Initializers without side effects.  */
  CHECK ((int) { 2 } += 0, 2, 0);
  CHECK ((int) { 2 } <<= 1, 4, 0);

  /* Reads that don't write the literal.  */
  CHECK (((void) (int) { val (1) }, 2), 2, 1);
  {
    int a[3] = { [0 ... 2] = (int) { val (1) } += 0 };
    if (a[0] != 1 || a[1] != 1 || a[2] != 1)
      abort ();
  }

  /* Each iteration still evaluates the initializer once.  */
  CHECK (loop_identity (5), 10, 5);

  return 0;
}
