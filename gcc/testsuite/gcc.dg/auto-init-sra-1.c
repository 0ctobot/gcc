/* Verify that SRA total scalarization will not be confused by padding
   and also not confused by auto initialization.  */
/* { dg-do compile } */
/* { dg-options "-O1 --param sra-max-scalarization-size-Ospeed=16 -fdump-tree-release_ssa -ftrivial-auto-var-init=zero" } */

struct S
{
  int i;
  unsigned short f1;
  char f2;
  unsigned short f3, f4;
};

struct T
{
  int i;
  unsigned short f1;
  char f2, f2b;
  unsigned short f3, f4;
};


int foo (struct S *p)
{
  struct S l;

  l = *p;
  l.i++;
  *p = l;
}

int bar (struct T *p)
{
  struct T m;

  m = *p;
  m.i++;
  *p = m;
}

/* With -ftrivial-auto-var-init=zero, SRA keeps an aggregate with padding
   whole, since replacing it with its fields would drop the padding bytes
   that the copy carries.  struct T has no padding and is still totally
   scalarized.  */
/* { dg-final { scan-tree-dump-times "\\*p_\[0-9\]+\\(D\\) = l;" 1 "release_ssa" } } */
/* { dg-final { scan-tree-dump-times "m;" 0 "release_ssa" } } */
