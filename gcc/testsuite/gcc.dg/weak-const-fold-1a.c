/* { dg-do compile } */

union U { int i; struct { short lo, hi; } s; };

/* These replace the weak definitions in weak-const-fold-1.c.  */
const int x = 50;
const union U wu = { 0x55667788 };
