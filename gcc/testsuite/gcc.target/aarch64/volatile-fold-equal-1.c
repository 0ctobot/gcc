/* { dg-do compile } */
/* { dg-options "-O2" } */

/* Each function reads one location through a plain lvalue and through a
   volatile one.  rtx_equal_p doesn't compare MEM_VOLATILE_P, so once
   combine substitutes the volatile load, the folds for equal operands must
   not take the two reads for one value and drop the volatile read.  */

#define READ_ONCE(x) (*(const volatile __typeof__ (x) *) &(x))

struct S { int a; unsigned long seq; int cnt; };
void ext (void);

int
sub (int *p)
{
  int a = *p;
  int b = READ_ONCE (*p);
  return a - b;
}

int
eq (struct S *s)
{
  int old = s->cnt;
  int cur = READ_ONCE (s->cnt);
  return old == cur;
}

int
lt (struct S *s)
{
  int old = s->cnt;
  int cur = READ_ONCE (s->cnt);
  return old < cur;
}

int
ne (int *p)
{
  int b = READ_ONCE (*p);
  int a = *p;
  return a != b;
}

void
recheck (struct S *s)
{
  unsigned long old = s->seq;
  unsigned long new = READ_ONCE (s->seq);
  if (old != new)
    ext ();
}

/* Two loads in each of the five functions, and the call in recheck.  */
/* { dg-final { scan-assembler-times {\tldr\t} 10 } } */
/* { dg-final { scan-assembler-times {\tbl?\text\M} 1 } } */
