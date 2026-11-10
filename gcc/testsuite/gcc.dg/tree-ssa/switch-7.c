/* { dg-do compile { target { { x86_64-*-* aarch64-*-* ia64-*-* powerpc64-*-* } && lp64 } } } */
/* { dg-options "-O2 -fdump-tree-switchlower1" } */

/* A switch lowered to a single jump table must keep the probabilities its
   edges had on entry instead of getting probabilities from the number of
   table slots each case covers.  The dump prints each switch on entry and
   again as the gswitch of the jump table, so each probability below is
   expected twice.  */

void h0 (void), h1 (void), h2 (void), h3 (void), h4 (void), h5 (void);
void h6 (void), h7 (void), h8 (void), h9 (void), h10 (void), h11 (void);
void rare (void);
void bad (void) __attribute__ ((noreturn));

#define CASES				\
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
  case 0x3c: h11 (); break;

/* A sparse table whose default ends in a noreturn call: the default stays
   unlikely rather than taking the share of the holes.  */
void
sparse_noreturn (unsigned int x)
{
  switch (x & 63)
    {
    CASES
    default: bad ();
    }
}

/* A sparse table with an expected case: that case stays likely.  */
void
sparse_expect (unsigned int x)
{
  switch ((unsigned int) __builtin_expect (x & 63, 0x15))
    {
    CASES
    default: rare (); break;
    }
}

/* A table without holes whose default is expected: the default stays
   likely rather than becoming impossible.  */
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

/* { dg-final { scan-tree-dump-times ";; GIMPLE switch case clusters: JT:0-60 " 2 "switchlower1" } } */
/* { dg-final { scan-tree-dump-times ";; GIMPLE switch case clusters: JT:0-11 " 1 "switchlower1" } } */
/* { dg-final { scan-tree-dump-times "default: <L\[0-9\]+> \\\[0\\.05%\\\]" 2 "switchlower1" } } */
/* { dg-final { scan-tree-dump-times "case 21: <L\[0-9\]+> \\\[90\\.00%\\\]" 2 "switchlower1" } } */
/* { dg-final { scan-tree-dump-times "default: <L\[0-9\]+> \\\[90\\.00%\\\]" 2 "switchlower1" } } */
