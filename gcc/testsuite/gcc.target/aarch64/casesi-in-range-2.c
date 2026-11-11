/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-require-effective-target lp64 } */

/* When the range of the index keeps it within the case values of a jump
   table, the bounds check of the casesi pattern can't fire and shouldn't
   be emitted.  Each table has a hole at 5, which the table itself sends to
   the default label, and each function uses a different table size so the
   scans can tell the checks apart.  */

void h0 (void), h1 (void), h2 (void), h3 (void), h4 (void), h5 (void);
void h6 (void), h7 (void), h8 (void), h9 (void), h10 (void), h11 (void);
void h12 (void), h13 (void), h14 (void), h15 (void), h16 (void);
void h17 (void), h18 (void), h19 (void), h20 (void), h21 (void);
void h22 (void), h23 (void), even (void), odd (void), rare (void);

#define CASES_0_19				\
  case 0: h0 (); break;				\
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
  case 12: h12 (); break;			\
  case 13: h13 (); break;			\
  case 14: h14 (); break;			\
  case 15: h15 (); break;			\
  case 16: h16 (); break;			\
  case 17: h17 (); break;			\
  case 18: h18 (); break;			\
  case 19: h19 (); break;

/* Switch lowering splits off the high case values with a check of the
   index against 23, which leaves the index in [0, 23] at the jump
   table.  */
void
split (unsigned long x)
{
  switch (x)
    {
    CASES_0_19
    case 20: h20 (); break;
    case 21: h21 (); break;
    case 22: h22 (); break;
    case 23: h23 (); break;
    case 100: case 102: case 104: case 106: case 108: even (); break;
    case 101: case 103: case 105: case 107: case 109: odd (); break;
    default: rare (); break;
    }
}

/* A check in the source leaves the index in [0, 22].  */
void
guarded (unsigned long x)
{
  if (x > 22)
    return;
  switch (x)
    {
    CASES_0_19
    case 20: h20 (); break;
    case 21: h21 (); break;
    case 22: h22 (); break;
    default: rare (); break;
    }
}

/* An index narrower than SImode in [-3, 14], which the casesi pattern
   checks after subtracting -3.  */
void
narrow (signed char c)
{
  if (c < -3 || c > 14)
    return;
  switch (c)
    {
    case -3: h20 (); break;
    case -2: h21 (); break;
    case -1: h22 (); break;
    case 0: h0 (); break;
    case 1: h1 (); break;
    case 2: h2 (); break;
    case 3: h3 (); break;
    case 4: h4 (); break;
    case 6: h6 (); break;
    case 7: h7 (); break;
    case 8: h8 (); break;
    case 9: h9 (); break;
    case 10: h10 (); break;
    case 11: h11 (); break;
    case 12: h12 (); break;
    case 13: h13 (); break;
    case 14: h14 (); break;
    default: rare (); break;
    }
}

/* { dg-final { scan-assembler-times {\tcmp\tx[0-9]+, 23\n} 1 } } */
/* { dg-final { scan-assembler-not {\tcmp\tw[0-9]+, 23\n} } } */
/* { dg-final { scan-assembler-times {\tcmp\tx[0-9]+, 22\n} 1 } } */
/* { dg-final { scan-assembler-not {\tcmp\tw[0-9]+, 22\n} } } */
/* { dg-final { scan-assembler-times {\tcmp\tw[0-9]+, 17\n} 1 } } */
