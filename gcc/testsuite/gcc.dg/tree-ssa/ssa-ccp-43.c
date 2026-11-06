/* { dg-do compile } */
/* { dg-options "-O -ftrivial-auto-var-init=zero -fdump-tree-ccp1" } */

int foo (int flag)
{
  int i;
  if (flag)
    i = 1;
  return i;
}

/* i is zero when flag is zero, so the return can't fold to the assigned value.  */
/* { dg-final { scan-tree-dump-not "return 1;" "ccp1" } } */
