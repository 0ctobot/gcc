/* PR middle-end/127752 */
/* Keeping volatile reads of a read-only object must not stop non-volatile
   reads of the same object from folding to the initializer.  */
/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-flto" } { "" } } */
/* { dg-additional-options "-g0 -fno-section-anchors -fno-inline" } */

struct S { volatile short a; int b; };
struct N { struct S s; int z; };

/* The object is referenced only by the sibling reads, so it goes away.  */
static struct S c1 = { 1, 22136 };
int f1 (void) { return c1.b; }

/* The volatile read needs the object, and the sibling read still folds.  */
static struct S c2 = { 1, 22137 };
int f2 (void) { c2.a; return c2.b; }

/* Likewise, for a sibling of the struct containing the volatile member.  */
static struct N c3 = { { 1, 2 }, 22138 };
int f3 (void) { c3.s.a; return c3.z; }

/* The range of a load from a plain read-only array is still known.  */
static const int c4[3] = { 1, 2, 3 };
int f4 (int i) { return c4[i] > 0; }

/* { dg-final { scan-assembler {\mmov\s+w[0-9]+, 22136\M} } } */
/* { dg-final { scan-assembler {\mmov\s+w[0-9]+, 22137\M} } } */
/* { dg-final { scan-assembler {\mmov\s+w[0-9]+, 22138\M} } } */
/* { dg-final { scan-assembler-not {:lo12:c1\M} } } */
/* { dg-final { scan-assembler {:lo12:c2\M} } } */
/* { dg-final { scan-assembler {:lo12:c3\M} } } */
/* { dg-final { scan-assembler-not {:lo12:c4\M} } } */
