/* { dg-do compile } */
/* { dg-options "-O2" } */

/* The bounds check of a jump table must branch to an unlikely default and
   fall through into the dispatch, and the other way around for a likely
   default.  The casesi pattern emits that check itself, so it has to get
   the default probability of the switch instead of an even guess.  Each
   function uses a different table size so the scans can tell the checks
   apart.  */

void h0 (void), h1 (void), h2 (void), h3 (void), h4 (void), h5 (void);
void h6 (void), h7 (void), h8 (void), h9 (void), h10 (void), h11 (void);
void rare (void);
void bad (void) __attribute__ ((noreturn));

#define CASES(LAST)			\
  case 0x00: h0 (); break;		\
  case 0x07: h1 (); break;		\
  case 0x0d: h2 (); break;		\
  case 0x15: h3 (); break;		\
  case 0x18: h4 (); break;		\
  case 0x1c: h5 (); break;		\
  case 0x20: h6 (); break;		\
  case 0x24: h7 (); break;		\
  case 0x2c: h8 (); break;		\
  case 0x30: h9 (); break;		\
  case 0x34: h10 (); break;		\
  case LAST: h11 (); break;

/* The default ends in a noreturn call.  */
void
sparse_noreturn (unsigned int x)
{
  switch (x & 63)
    {
    CASES (60)
    default: bad ();
    }
}

/* One case is expected.  */
void
sparse_expect (unsigned int x)
{
  switch ((unsigned int) __builtin_expect (x & 63, 0x15))
    {
    CASES (61)
    default: rare (); break;
    }
}

/* The default is expected and the table has no holes.  */
void
dense_expect_default (unsigned int x)
{
  switch ((unsigned int) __builtin_expect (x & 63, 40))
    {
    case 0: h0 (); break;
    case 1: h1 (); break;
    case 2: h2 (); break;
    case 3: h3 (); break;
    case 4: h4 (); break;
    case 5: h5 (); break;
    case 6: h6 (); break;
    case 7: h7 (); break;
    case 8: h8 (); break;
    case 9: h9 (); break;
    case 10: h10 (); break;
    case 11: h11 (); break;
    default: rare (); break;
    }
}

/* { dg-final { scan-assembler-times {\tcmp\tw[0-9]+, 60\n\tbhi\t} 1 } } */
/* { dg-final { scan-assembler-times {\tcmp\tw[0-9]+, 61\n\tbhi\t} 1 } } */
/* { dg-final { scan-assembler-times {\tcmp\tw[0-9]+, 11\n\tbls\t} 1 } } */
