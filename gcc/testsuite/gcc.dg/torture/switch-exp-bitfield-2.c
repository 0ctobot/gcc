/* { dg-do run } */

/* The same as switch-exp-bitfield-1.c for an unsigned 3-bit field inside a
   loop.  Once loop unswitching moves the conversion of the field to int out
   of the loop, its range is unknown where the jump threader looks at it, so
   the range of .FFS is [0, 32].  Built in the 3-bit type of the field, that
   range wrapped to [0, 0] and the case lookup was folded away.  */

struct S { unsigned int bf3 : 3; };
struct S gs[64];
int g54;
unsigned int gbrk;

__attribute__ ((noipa)) int
f (void)
{
  unsigned int x = g54;
  int sw = 0, i = 0;

  do
    {
      if (gbrk & 1)
	break;
      switch (gs[(x ^ 1) & 63].bf3)
	{
	default:
	  sw = 0;
	  break;
	case 2:
	  sw = 1;
	  break;
	case 1:
	  sw = 7;
	  break;
	case 4:
	  sw = -32;
	}
    }
  while (++i != 82);
  return sw;
}

int
main (void)
{
  static const int expect[8] = { 0, 7, 1, 0, -32, 0, 0, 0 };

  for (int v = 0; v < 8; v++)
    {
      gs[1].bf3 = v;
      if (f () != expect[v])
	__builtin_abort ();
    }
  return 0;
}
