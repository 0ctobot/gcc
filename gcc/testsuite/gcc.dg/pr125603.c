/* PR125603 */
/* { dg-do compile } */
/* { dg-options "-O1 -fsanitize=undefined -w" } */

_Bool c;

void
foo (void)
{
  long long v1;
  signed char v2;
  short v3;
  if (v2 == 0)
    goto b7;
  v3 = 40;
  goto b11;
b7:
  __builtin_ctzll (v1);
  v3 = 0;
b11:
  v1 = 0;
  switch (v3)
    {
    case 7:
      goto sw15;
    case 3:
      goto sw15;
    case 4080:
      __builtin_unreachable ();
    case 0:
      goto b11;
    default:
      goto b7;
    }
sw15:
  if (c)
    goto sw15;
  __builtin_unreachable ();
}
