/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-ifcombine-details" } */

/* Merging two compares into one wide load mustn't create a load that
   covers a volatile member the source never reads.  */

struct A { unsigned char a; volatile unsigned char v; unsigned char b;
	   unsigned char c; unsigned int x; };
struct A ga;

int
f1 (void)
{
  return ga.a == 1 && ga.b == 2;
}

/* The volatile member can also sit inside a nested aggregate.  */

struct B { unsigned char a; struct { volatile unsigned char v; } in;
	   unsigned char b; unsigned char c; unsigned int x; };
struct B gb;

int
f2 (void)
{
  return gb.a == 1 && gb.b == 2;
}

/* A volatile member outside of the combined load doesn't matter.  */

struct C { unsigned char a; unsigned char c; unsigned char b;
	   unsigned char d; volatile unsigned int v; };
struct C gc;

int
f3 (void)
{
  return gc.a == 1 && gc.b == 2;
}

/* The same applies to the load on the right-hand side of the compares.  */

struct D { unsigned char a; unsigned char v; unsigned char b;
	   unsigned char c; unsigned int x; };
struct D gd;

int
f4 (void)
{
  return gd.a == ga.a && gd.b == ga.b;
}

/* { dg-final { scan-tree-dump-times "optimizing two comparisons" 1 "ifcombine" { target { ! { avr-*-* pru-*-* } } } } } */
