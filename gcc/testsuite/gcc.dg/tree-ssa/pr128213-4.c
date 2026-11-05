/* PR tree-optimization/128213 */
/* { dg-do compile } */
/* { dg-options "-O1 -fgimple -fdump-tree-phiprop1-details" } */

/* The load goes through a copy of the PHI result, which the pass follows
   before it looks at the type of the load, and it runs past the end of ch
   only when d is nonzero.  */

_Alignas (int) char ch[1];
int iv;

int __GIMPLE (ssa,startwith ("phiprop1"))
f (int c, int d)
{
  int *p;
  int *q;
  int r;

  __BB(2):
  if (c_1(D) != 0)
    goto __BB3;
  else
    goto __BB4;

  __BB(3):
  goto __BB4;

  __BB(4):
  p_3 = __PHI (__BB2: &iv, __BB3: _Literal (int *) &ch);
  q_4 = p_3;
  if (d_2(D) != 0)
    goto __BB5;
  else
    goto __BB6;

  __BB(5):
  r_5 = __MEM <int> (q_4);
  goto __BB6;

  __BB(6):
  r_6 = __PHI (__BB4: 0, __BB5: r_5);
  return r_6;
}

/* { dg-final { scan-tree-dump-not "Inserting PHI for result of load" "phiprop1" } } */
