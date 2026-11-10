/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-ifcombine-details" } */

/* Checking the merged load for volatile members mustn't walk a type once
   per path through it.  Each union repeats its member type, so U40 has 2^41
   paths to its leaves.  */

#define U(n, m) typedef union U##n { U##m a, b; } U##n;

typedef union U0 { unsigned char a, b; } U0;
U (1, 0) U (2, 1) U (3, 2) U (4, 3) U (5, 4)
U (6, 5) U (7, 6) U (8, 7) U (9, 8) U (10, 9)
U (11, 10) U (12, 11) U (13, 12) U (14, 13) U (15, 14)
U (16, 15) U (17, 16) U (18, 17) U (19, 18) U (20, 19)
U (21, 20) U (22, 21) U (23, 22) U (24, 23) U (25, 24)
U (26, 25) U (27, 26) U (28, 27) U (29, 28) U (30, 29)
U (31, 30) U (32, 31) U (33, 32) U (34, 33) U (35, 34)
U (36, 35) U (37, 36) U (38, 37) U (39, 38) U (40, 39)

struct P { unsigned char a; U40 u; unsigned char b, c; }
  __attribute__ ((aligned (4)));
struct P p;

int
f (void)
{
  return p.a == 1 && p.b == 2;
}

/* { dg-final { scan-tree-dump-times "optimizing two comparisons" 1 "ifcombine" { target { ! { avr-*-* pru-*-* } } } } } */
