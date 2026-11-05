/* { dg-do run } */
/* { dg-additional-options "-funswitch-loops" } */

#define C2(x) case (x): case (x) + 2:
#define C4(x) C2(x) C2((x) + 4)
#define C8(x) C4(x) C4((x) + 8)
#define C16(x) C8(x) C8((x) + 16)
#define C32(x) C16(x) C16((x) + 32)
#define C64(x) C32(x) C32((x) + 64)
#define C128(x) C64(x) C64((x) + 128)
#define C256(x) C128(x) C128((x) + 256)

volatile int v;

/* The range of the edge taken by the 256 labels can only hold 255 sub-ranges,
   so it swallows 509, which must still reach the default arm.  */
__attribute__((noipa)) int
if_probe (unsigned a, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    {
      switch (a)
	{
	C256 (0)
	  s += 1;
	  break;
	default:
	  s += 2;
	  if (a == 509)
	    s += 100;
	  break;
	}
      v++;
    }
  return s;
}

/* The same, with a second switch checking for the swallowed value.  */
__attribute__((noipa)) int
switch_probe (unsigned a, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    {
      switch (a)
	{
	C256 (0)
	  s += 1;
	  break;
	default:
	  s += 2;
	  switch (a)
	    {
	    case 509:
	      s += 100;
	      break;
	    case 513:
	      s += 200;
	      break;
	    }
	  break;
	}
      v++;
    }
  return s;
}

/* The same, at the top of the type instead of the bottom.  */
__attribute__((noipa)) int
top_of_type (unsigned a, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    {
      switch (a)
	{
	C256 (0xfffffe01u)
	  s += 1;
	  break;
	default:
	  s += 2;
	  if (a == 0xfffffffeu)
	    s += 100;
	  break;
	}
      v++;
    }
  return s;
}

/* The same, with another edge that's still fine to unswitch on.  */
__attribute__((noipa)) int
other_edge (unsigned a, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    {
      switch (a)
	{
	C256 (0)
	  s += 1;
	  break;
	case 1000:
	  s += 5;
	  break;
	default:
	  s += 2;
	  if (a == 509)
	    s += 100;
	  break;
	}
      v++;
    }
  return s;
}

int
main (void)
{
  if (if_probe (509, 3) != 306
      || if_probe (510, 3) != 3
      || if_probe (511, 3) != 6)
    __builtin_abort ();
  if (switch_probe (509, 3) != 306
      || switch_probe (513, 3) != 606
      || switch_probe (510, 3) != 3
      || switch_probe (511, 3) != 6)
    __builtin_abort ();
  if (top_of_type (0xfffffffeu, 3) != 306
      || top_of_type (0xffffffffu, 3) != 3
      || top_of_type (0xfffffffcu, 3) != 6)
    __builtin_abort ();
  if (other_edge (509, 3) != 306
      || other_edge (1000, 3) != 15
      || other_edge (510, 3) != 3
      || other_edge (511, 3) != 6)
    __builtin_abort ();
  return 0;
}
