/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-ter" } */

/* As in volatile-fold-equal-1.c, for the IOR, XOR and AND folds of equal
   operands.  Without TER the plain load is expanded before the volatile
   one, so the volatile copy ends up as operand 1 of the operation.  */

#define READ_ONCE(x) (*(const volatile __typeof__ (x) *) &(x))

int
ior (int *p)
{
  int a = *p;
  int b = READ_ONCE (*p);
  return a | b;
}

int
xor (int *p)
{
  int a = *p;
  int b = READ_ONCE (*p);
  return a ^ b;
}

int
and (int *p)
{
  int a = *p;
  int b = READ_ONCE (*p);
  return a & b;
}

/* Two loads in each function.  */
/* { dg-final { scan-assembler-times {\tldr\t} 6 } } */
