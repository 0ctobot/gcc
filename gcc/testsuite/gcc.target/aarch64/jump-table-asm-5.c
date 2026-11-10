/* A jump table with no inline asm between its dispatch and its case labels
   keeps its byte entries, even when some cases are cold.  The same table
   with an inline asm in range gets word entries.  */
/* { dg-do compile } */
/* { dg-options "-O2" } */

extern void die (int) __attribute__ ((cold, noreturn));

#define CASES(X)						\
  case 0: X; r = a * 3 + b; break;				\
  case 1: X; r = a * 5 + b; break;				\
  case 2: X; r = a * 7 + b; break;				\
  case 3: die (3);						\
  case 4: X; r = a * 11 + b; break;				\
  case 5: X; r = a * 13 + b; break;				\
  case 6: X; r = a * 15 + b; break;				\
  case 7: X; r = a * 17 + b; break;				\
  case 8: die (8);						\
  case 9: X; r = a * 21 + b; break;				\
  case 10: X; r = a * 23 + b; break;				\
  case 11: X; r = a * 25 + b; break;				\
  case 12: X; r = a * 27 + b; break;				\
  case 13: die (13);						\
  case 14: X; r = a * 31 + b; break;				\
  case 15: X; r = a * 33 + b; break;				\
  case 16: X; r = a * 35 + b; break;				\
  default: r = -1; break;

long
f (unsigned c, long a, long b)
{
  long r;
  switch (c)
    {
      CASES ((void) 0)
    }
  return r;
}

long
h (unsigned c, long a, long b)
{
  long r;
  switch (c)
    {
      CASES (asm volatile ("nop"))
    }
  return r;
}

/* { dg-final { scan-assembler-times {\.byte\t\(\.L[0-9]+ - \.Lrtx[0-9]+\) / 4} 17 } } */
/* { dg-final { scan-assembler-times {\.word\t\(\.L[0-9]+ - \.Lrtx[0-9]+\) / 4} 17 } } */
