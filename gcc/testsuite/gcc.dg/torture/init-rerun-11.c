/* { dg-do run } */

/* A jump at or below the declaration that comes back above it runs the
   initializer again, however it gets there.  The call in the initializer must
   then read the old value through the pointer.  */

struct T { int k; int pad0; long a[6]; };

__attribute__ ((noipa)) long
g (long x)
{
  return x;
}

__attribute__ ((noipa)) long
second (int a, struct T *b)
{
  return b->a[3] + a - 1;
}

/* The jump back to mid goes on to a jump above the declaration.  */

__attribute__ ((noipa)) long
through_label (long n)
{
  struct T *p = 0;
top:
  g (0);
  goto skip;
mid:
  goto top;
skip:;
  struct T v = { .k = n + 1, .a[3] = g (p ? p->k : -1) };
  p = &v;
  if (n++ < 1)
    goto mid;
  return v.a[3];
}

__attribute__ ((noipa)) long
literal (long n)
{
  struct T *p = 0;
  struct T *q;
again:
  q = &(struct T) { .k = n + 1, .a[3] = g (p ? p->k : -1) };
  p = q;
  if (n++ < 1)
    goto again;
  return q->a[3];
}

/* If the literal goes first, the jump comes after it and the second run reads
   1.  If the jump goes first, the only run has no old value and reads -1.  */

__attribute__ ((noipa)) long
literal_operand (long n)
{
  struct T *p = 0;
  struct T *q = 0;
again:;
  long r = second (({ if (n++ < 1) { p = q; goto again; } 1; }),
		   q = &(struct T) { .k = n + 1, .a[3] = g (p ? p->k : -1) });
  return r;
}

int
main (void)
{
  if (through_label (0) != 1)
    __builtin_abort ();
  if (literal (0) != 1)
    __builtin_abort ();
  long r = literal_operand (0);
  if (r != 1 && r != -1)
    __builtin_abort ();
  return 0;
}
