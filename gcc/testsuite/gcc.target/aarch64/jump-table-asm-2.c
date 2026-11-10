/* Like jump-table-asm-1.c, but with asm goto, which counts as an inline asm
   when choosing the jump table entry size just like any other asm.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#define NOPS(n) asm goto (".rept " #n "\nnop\n.endr" : : : : bail)

__attribute__ ((noipa)) long
f (unsigned c, long a, long b)
{
  long r;
  switch (c)
    {
    case 0: NOPS (6); r = a * 3 + b; break;
    case 1: NOPS (6); r = a * 5 + b; break;
    case 2: NOPS (6); r = a * 7 + b; break;
    case 3: NOPS (6); r = a * 9 + b; break;
    case 4: NOPS (6); r = a * 11 + b; break;
    case 5: NOPS (6); r = a * 13 + b; break;
    case 6: NOPS (6); r = a * 15 + b; break;
    case 7: NOPS (6); r = a * 17 + b; break;
    case 8: NOPS (6); r = a * 19 + b; break;
    case 9: NOPS (6); r = a * 21 + b; break;
    case 10: NOPS (6); r = a * 23 + b; break;
    case 11: NOPS (6); r = a * 25 + b; break;
    case 12: NOPS (6); r = a * 27 + b; break;
    case 13: NOPS (6); r = a * 29 + b; break;
    case 14: NOPS (6); r = a * 31 + b; break;
    case 15: NOPS (6); r = a * 33 + b; break;
    case 16: NOPS (6); r = a * 35 + b; break;
    default: r = -1; break;
    }
  return r;
bail:
  return -2;
}

int
main (void)
{
  for (unsigned c = 0; c < 18; c++)
    {
      long e = c < 17 ? (long) (12 * (c * 2 + 3) + 5) : -1L;
      if (f (c, 12, 5) != e)
	__builtin_abort ();
    }
  return 0;
}
