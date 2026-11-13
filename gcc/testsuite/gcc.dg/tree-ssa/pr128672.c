/* PR tree-optimization/128672 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-fre1" } */

int g;

/* With undefined overflow the program's own negation shows that the
   negated operand isn't INT_MIN, so FRE computes one division per
   function here.  */

int f1 (int a, int b) { g = a / b; return -a / b; }
int f2 (int a, int b) { g = -a / b; return a / b; }
int f3 (int a) { g = a / 3; return a / -3; }
int f4 (int a, int b) { g = a / b; return a / -b; }
int f5 (int a, int b) { g = a / -b; return a / b; }

/* Here -(int) -X is only known to equal (int) X, which may be INT_MIN,
   so both divisions have to stay.  */

int f6 (unsigned x) { g = (int) x / 2; return (int) -x / 2; }

/* { dg-final { scan-tree-dump-times " / " 7 "fre1" } } */
