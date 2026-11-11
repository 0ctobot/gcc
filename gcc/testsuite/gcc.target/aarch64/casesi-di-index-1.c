/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-require-effective-target lp64 } */

/* A jump table indexed by a 64-bit value needs a bounds check in DImode
   only when the range of the index minus the lowest case value doesn't fit
   in 32 bits, and the SImode check of the casesi pattern is enough
   otherwise.  An index that gets past the DImode check is in the table, so
   the SImode check isn't needed after it.  Each function uses a different
   table size so the scans can tell the checks apart.  */

void h0 (void), h1 (void), h2 (void), h3 (void), h4 (void), h5 (void);
void h6 (void), h7 (void), h8 (void), h9 (void), h10 (void), h11 (void);
void h12 (void), h13 (void), h14 (void), h15 (void), h16 (void);
void h17 (void), h18 (void), h19 (void), rare (void);

#define CASES_0_11				\
  case 0: h0 (); break;				\
  case 1: h1 (); break;				\
  case 2: h2 (); break;				\
  case 3: h3 (); break;				\
  case 4: h4 (); break;				\
  case 5: h5 (); break;				\
  case 6: h6 (); break;				\
  case 7: h7 (); break;				\
  case 8: h8 (); break;				\
  case 9: h9 (); break;				\
  case 10: h10 (); break;			\
  case 11: h11 (); break;

#define CASES_12_15				\
  case 12: h12 (); break;			\
  case 13: h13 (); break;			\
  case 14: h14 (); break;			\
  case 15: h15 (); break;

/* The index is at most 63: the SImode check.  */
void
masked (unsigned long esr)
{
  switch ((esr >> 26) & 63)
    {
    CASES_0_11
    default: rare (); break;
    }
}

/* The index has no known range: the DImode check.  */
void
full (unsigned long x)
{
  switch (x)
    {
    CASES_0_11
    case 12: h12 (); break;
    default: rare (); break;
    }
}

/* The index minus the lowest case value is in [-2, 61]: the SImode
   check.  */
void
rebased (unsigned long x)
{
  switch (x & 63)
    {
    case 2: h0 (); break;
    case 3: h1 (); break;
    case 4: h2 (); break;
    case 5: h3 (); break;
    case 6: h4 (); break;
    case 7: h5 (); break;
    case 8: h6 (); break;
    case 9: h7 (); break;
    case 10: h8 (); break;
    case 11: h9 (); break;
    case 12: h10 (); break;
    case 13: h11 (); break;
    case 14: h12 (); break;
    case 15: h13 (); break;
    default: rare (); break;
    }
}

/* The index can exceed 32 bits even though its range is known: the DImode
   check.  */
void
partial (unsigned long x)
{
  switch (x & 0x1ffffffffUL)
    {
    CASES_0_11
    case 12: h12 (); break;
    case 13: h13 (); break;
    case 14: h14 (); break;
    default: rare (); break;
    }
}

/* The index is at most 2^32 - 1: the SImode check.  */
void
hi_fits (unsigned long x)
{
  if (x > 0xffffffffUL)
    return;
  switch (x)
    {
    CASES_0_11
    CASES_12_15
    default: rare (); break;
    }
}

/* The index can be 2^32, which truncates to 0: the DImode check.  */
void
hi_past (unsigned long x)
{
  if (x > 0x100000000UL)
    return;
  switch (x)
    {
    CASES_0_11
    CASES_12_15
    case 16: h16 (); break;
    default: rare (); break;
    }
}

/* The index can be 18 - 2^32, which truncates to 18, one past the
   highest case value: the SImode check.  */
void
lo_fits (long v)
{
  if (v < 18 - 0x100000000L || v > 1000)
    return;
  switch (v)
    {
    CASES_0_11
    CASES_12_15
    case 16: h16 (); break;
    case 17: h17 (); break;
    default: rare (); break;
    }
}

/* The index can be 18 - 2^32, which truncates to the highest case value:
   the DImode check.  */
void
lo_past (long v)
{
  if (v < 18 - 0x100000000L || v > 1000)
    return;
  switch (v)
    {
    CASES_0_11
    CASES_12_15
    case 16: h16 (); break;
    case 17: h17 (); break;
    case 18: h18 (); break;
    default: rare (); break;
    }
}

/* A sign-extended int with a negative lowest case value: the SImode
   check.  */
void
sext (int i)
{
  long v = i;

  switch (v)
    {
    case -6: h0 (); break;
    case -5: h1 (); break;
    case -4: h2 (); break;
    case -3: h3 (); break;
    case -2: h4 (); break;
    case -1: h5 (); break;
    case 0: h6 (); break;
    case 1: h7 (); break;
    case 2: h8 (); break;
    case 3: h9 (); break;
    case 4: h10 (); break;
    case 5: h11 (); break;
    case 6: h12 (); break;
    case 7: h13 (); break;
    case 8: h14 (); break;
    case 9: h15 (); break;
    case 10: h16 (); break;
    case 11: h17 (); break;
    case 12: h18 (); break;
    case 13: h19 (); break;
    default: rare (); break;
    }
}

/* { dg-final { scan-assembler-times {\tcmp\tw[0-9]+, 11\n} 1 } } */
/* { dg-final { scan-assembler-not {\tcmp\tx[0-9]+, 11\n} } } */
/* { dg-final { scan-assembler-not {\tcmp\tw[0-9]+, 12\n} } } */
/* { dg-final { scan-assembler-times {\tcmp\tx[0-9]+, 12\n} 1 } } */
/* { dg-final { scan-assembler-times {\tcmp\tw[0-9]+, 13\n} 1 } } */
/* { dg-final { scan-assembler-not {\tcmp\tx[0-9]+, 13\n} } } */
/* { dg-final { scan-assembler-not {\tcmp\tw[0-9]+, 14\n} } } */
/* { dg-final { scan-assembler-times {\tcmp\tx[0-9]+, 14\n} 1 } } */
/* { dg-final { scan-assembler-times {\tcmp\tw[0-9]+, 15\n} 1 } } */
/* { dg-final { scan-assembler-not {\tcmp\tx[0-9]+, 15\n} } } */
/* { dg-final { scan-assembler-not {\tcmp\tw[0-9]+, 16\n} } } */
/* { dg-final { scan-assembler-times {\tcmp\tx[0-9]+, 16\n} 1 } } */
/* { dg-final { scan-assembler-times {\tcmp\tw[0-9]+, 17\n} 1 } } */
/* { dg-final { scan-assembler-not {\tcmp\tx[0-9]+, 17\n} } } */
/* { dg-final { scan-assembler-not {\tcmp\tw[0-9]+, 18\n} } } */
/* { dg-final { scan-assembler-times {\tcmp\tx[0-9]+, 18\n} 1 } } */
/* { dg-final { scan-assembler-times {\tcmp\tw[0-9]+, 19\n} 1 } } */
/* { dg-final { scan-assembler-not {\tcmp\tx[0-9]+, 19\n} } } */
