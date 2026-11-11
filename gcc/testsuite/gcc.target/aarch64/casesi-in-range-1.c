/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-require-effective-target lp64 } */

/* A jump table indexed by a 64-bit value with no known range gets a bounds
   check in DImode before the casesi pattern.  An index that gets past that
   check is in the table, so the SImode check of the casesi pattern can't
   fire and shouldn't be emitted.  Each function uses a different table
   size so the scans can tell the checks apart.  */

void h0 (void), h1 (void), h2 (void), h3 (void), h4 (void), h5 (void);
void h6 (void), h7 (void), h8 (void), h9 (void), h10 (void), h11 (void);
void h12 (void), h13 (void), h14 (void), h15 (void), rare (void);

#define CASES_1_12				\
  case 1: h1 (); break;				\
  case 2: h2 (); break;				\
  case 3: h3 (); break;				\
  case 4: h4 (); break;				\
  case 6: h6 (); break;				\
  case 7: h7 (); break;				\
  case 8: h8 (); break;				\
  case 9: h9 (); break;				\
  case 10: h10 (); break;			\
  case 11: h11 (); break;			\
  case 12: h12 (); break;

/* A table without holes.  */
void
full (unsigned long x)
{
  switch (x)
    {
    case 0: h0 (); break;
    CASES_1_12
    case 5: h5 (); break;
    case 13: h13 (); break;
    default: rare (); break;
    }
}

/* A hole at 5, which the table itself sends to the default label.  */
void
hole (unsigned long x)
{
  switch (x)
    {
    case 0: h0 (); break;
    CASES_1_12
    case 13: h13 (); break;
    case 14: h14 (); break;
    default: rare (); break;
    }
}

/* A negative lowest case value, which is subtracted before the checks.  */
void
neg (long v)
{
  switch (v)
    {
    case -3: h0 (); break;
    case -2: h13 (); break;
    case -1: h14 (); break;
    case 0: h15 (); break;
    CASES_1_12
    default: rare (); break;
    }
}

/* { dg-final { scan-assembler-times {\tcmp\tx[0-9]+, 13\n} 1 } } */
/* { dg-final { scan-assembler-not {\tcmp\tw[0-9]+, 13\n} } } */
/* { dg-final { scan-assembler-times {\tcmp\tx[0-9]+, 14\n} 1 } } */
/* { dg-final { scan-assembler-not {\tcmp\tw[0-9]+, 14\n} } } */
/* { dg-final { scan-assembler-times {\tcmp\tx[0-9]+, 15\n} 1 } } */
/* { dg-final { scan-assembler-not {\tcmp\tw[0-9]+, 15\n} } } */
