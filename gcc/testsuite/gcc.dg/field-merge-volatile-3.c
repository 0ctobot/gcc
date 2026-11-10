/* { dg-do compile { target lp64 } } */
/* { dg-options "-O2 -fdump-tree-ifcombine-details" } */

/* Checking the merged load for volatile members mustn't walk a type once
   per path through it, even when the load covers part of an array and each
   element is checked in full.  Each struct repeats its member type at a
   different offset, so S36 has 2^37 leaves, and it's too large for targets
   with 32-bit pointers.  */

#define S(n, m) typedef struct S##n { S##m a, b; } S##n;

typedef struct S0 { unsigned char a, b; } S0;
S (1, 0) S (2, 1) S (3, 2) S (4, 3) S (5, 4)
S (6, 5) S (7, 6) S (8, 7) S (9, 8) S (10, 9)
S (11, 10) S (12, 11) S (13, 12) S (14, 13) S (15, 14)
S (16, 15) S (17, 16) S (18, 17) S (19, 18) S (20, 19)
S (21, 20) S (22, 21) S (23, 22) S (24, 23) S (25, 24)
S (26, 25) S (27, 26) S (28, 27) S (29, 28) S (30, 29)
S (31, 30) S (32, 31) S (33, 32) S (34, 33) S (35, 34)
S (36, 35)

struct P { unsigned char a, c, b; S36 s[1]; } __attribute__ ((aligned (4)));
extern struct P p;

int
f (void)
{
  return p.a == 1 && p.b == 2;
}

/* { dg-final { scan-tree-dump-times "optimizing two comparisons" 1 "ifcombine" } } */
