/* PR tree-optimization/128402 */
/* { dg-do run { target bitint } } */
/* { dg-options "-std=gnu23" } */
/* { dg-additional-options "-fno-stack-protector" } */

#if __BITINT_MAXWIDTH__ >= 129
_BitInt(129) x, y;
int c, n = 1;

void
f (_BitInt(129) a, _BitInt(129) b)
{
  if (n--)
    {
      x = 0;
      f (c ? x : 0x10000000000000001uwb, y);
    }
  y += a;
}
#endif

int
main ()
{
#if __BITINT_MAXWIDTH__ >= 129
  f (0, 0);
  if (y != 0x10000000000000001uwb)
    __builtin_abort ();
#endif
  return 0;
}
