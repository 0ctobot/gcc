/* PR tree-optimization/126565 */
/* { dg-do run } */
/* { dg-options "-O1 -fgimple" } */

/* The VIEW_CONVERT_EXPR operand already has the unsigned type, and the final
   cast here is to that unsigned type as well.  g_7 is 1 for less and 255 for
   greater, so this isn't an order reversal.  */

_Bool __GIMPLE (ssa, startwith ("phiopt1"))
foo (int x, int y)
{
  signed char c;
  unsigned char d;
  unsigned char e;
  unsigned char g;
  _Bool r;

  __BB(2):
  if (x_2(D) == y_3(D))
    goto __BB5;
  else
    goto __BB3;

  __BB(3):
  if (x_2(D) < y_3(D))
    goto __BB5;
  else
    goto __BB4;

  __BB(4):
  goto __BB5;

  __BB(5):
  c_4 = __PHI (__BB2: _Literal (signed char) 0, __BB3: _Literal (signed char) -1, __BB4: _Literal (signed char) 1);
  d_5 = __VIEW_CONVERT <unsigned char> (c_4);
  e_6 = -d_5;
  g_7 = __VIEW_CONVERT <unsigned char> (e_6);
  r_8 = g_7 > _Literal (unsigned char) 0;
  return r_8;
}

__attribute__((noipa)) _Bool
bar (int x, int y)
{
  return foo (x, y);
}

int
main ()
{
  for (int i = -1; i <= 1; i++)
    for (int j = -1; j <= 1; j++)
      if (bar (i, j) != (i != j))
	__builtin_abort ();
  return 0;
}
