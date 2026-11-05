/* PR middle-end/127752 */
/* Volatile reads of an object that's never written must load the object itself
   and must use the loaded value, even though the object gets promoted to
   read-only or was declared const.  */
/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-flto" } { "" } } */
/* { dg-additional-options "-g0 -fno-section-anchors -fno-inline" } */

extern void abort (void);

struct S { volatile short a; int b; };
struct T { short a; int b; };
struct N { int x; struct S s; };
struct A { int x; volatile int arr[2]; };
struct B { volatile unsigned f : 15; int b; };
struct I { volatile int a; };
struct C { const volatile short a; int b; };
union U { volatile short a; int b; };
struct __attribute__ ((scalar_storage_order ("big-endian"))) E
{
  volatile short a;
  int b;
};

/* Discarded value.  */
static struct S c1 = { 4660, 2 };
void f1 (void) { c1.a; }

/* Returned value.  */
static struct S c2 = { 4660, 2 };
int f2 (void) { return c2.a; }

/* Bit-field.  */
static struct B c3 = { 4660, 2 };
int f3 (void) { return c3.f; }

/* Nested member.  */
static struct N c4 = { 1, { 4660, 2 } };
int f4 (void) { return c4.s.a; }

/* Array element of a member.  */
static struct A c5 = { 1, { 2, 4660 } };
int f5 (void) { return c5.arr[1]; }

/* Member of an array element.  */
static struct S c6[2] = { { 1, 2 }, { 4660, 2 } };
int f6 (void) { return c6[1].a; }

/* Const volatile member.  */
static struct C c7 = { 4660, 2 };
int f7 (void) { return c7.a; }

/* Object declared const, which isn't promoted by IPA.  */
static const struct S c8 = { 4660, 2 };
int f8 (void) { return c8.a; }

/* Union member.  */
static union U c9 = { 4660 };
int f9 (void) { return c9.a; }

/* Volatile access to an object without volatile members.  */
static struct T c10 = { 4660, 2 };
int f10 (void) { return ((volatile struct T *) &c10)->a; }

/* Volatile array access to an object without volatile elements.  */
static int c11[2] = { 1, 4660 };
int f11 (void) { return (*(volatile int (*)[2]) &c11)[1]; }

/* Likewise, for a character array with a string initializer.  */
static char c12[4] = "\x1\x34\x3";
int f12 (void) { return (*(volatile char (*)[4]) &c12)[1]; }

/* Struct small enough to fit in a scalar mode.  */
static struct I c13 = { 4660 };
int f13 (void) { return c13.a; }

/* Reverse storage order.  */
static struct E c14 = { 4660, 2 };
int f14 (void) { return c14.a; }

/* Pointer parameter that IPA-CP replaces with the object's address.  */
static const struct S c15 = { 4660, 2 };
static int rd15 (const struct S *p) { return p->a; }
int f15 (void) { return rd15 (&c15) + rd15 (&c15); }

/* Polling loops.  */
static struct S c16 = { 0, 2 };
void f16 (void) { while (!c16.a) ; }
static struct S c17 = { 4660, 2 };
void f17 (void) { while (c17.a) ; }
static struct B c18 = { 4660, 2 };
void f18 (void) { while (c18.f) ; }
static struct T c19 = { 4660, 2 };
void f19 (void) { while (((volatile struct T *) &c19)->a) ; }
static int c20[2] = { 1, 4660 };
void f20 (void) { while ((*(volatile int (*)[2]) &c20)[1]) ; }
static struct A c21 = { 1, { 2, 4660 } };
void f21 (void) { while (c21.arr[1]) ; }

/* Comparison against the initial value.  */
static struct S c22 = { 1, 2 };
void f22 (void) { if (c22.a != 1) abort (); }

/* Store to a volatile member of an object without any other accesses.  */
static struct S c23 = { 1, 2 };
void f23 (void) { c23.a = 5; }

/* { dg-final { scan-assembler {:lo12:c1\M} } } */
/* { dg-final { scan-assembler {:lo12:c2\M} } } */
/* { dg-final { scan-assembler {:lo12:c3\M} } } */
/* { dg-final { scan-assembler {:lo12:c4\M} } } */
/* { dg-final { scan-assembler {:lo12:c5\M} } } */
/* { dg-final { scan-assembler {:lo12:c6\M} } } */
/* { dg-final { scan-assembler {:lo12:c7\M} } } */
/* { dg-final { scan-assembler {:lo12:c8\M} } } */
/* { dg-final { scan-assembler {:lo12:c9\M} } } */
/* { dg-final { scan-assembler {:lo12:c10\M} } } */
/* { dg-final { scan-assembler {:lo12:c11\M} } } */
/* { dg-final { scan-assembler {:lo12:c12\M} } } */
/* { dg-final { scan-assembler {:lo12:c13\M} } } */
/* { dg-final { scan-assembler {:lo12:c14\M} } } */
/* { dg-final { scan-assembler {:lo12:c15\M} } } */
/* { dg-final { scan-assembler {:lo12:c16\M} } } */
/* { dg-final { scan-assembler {:lo12:c17\M} } } */
/* { dg-final { scan-assembler {:lo12:c18\M} } } */
/* { dg-final { scan-assembler {:lo12:c19\M} } } */
/* { dg-final { scan-assembler {:lo12:c20\M} } } */
/* { dg-final { scan-assembler {:lo12:c21\M} } } */
/* { dg-final { scan-assembler {:lo12:c22\M} } } */
/* { dg-final { scan-assembler {:lo12:c23\M} } } */

/* The loaded value must be used instead of the initializer.  */
/* { dg-final { scan-assembler-not {\mmov\s+[wx][0-9]+, 4660\M} } } */
/* { dg-final { scan-assembler-not {\mmov\s+[wx][0-9]+, 13330\M} } } */
/* { dg-final { scan-assembler-not {\mmov\s+[wx][0-9]+, 52\M} } } */

/* A polling loop must not turn into a branch to itself.  */
/* { dg-final { scan-assembler-not {\.L[0-9]+:\s+b\s+\.L[0-9]+\n} } } */

/* The comparison must stay.  */
/* { dg-final { scan-assembler {\mabort\M} } } */

/* The count-zeroes table rewrite in forwprop must keep a volatile read of
   the table.  */
static const unsigned char c24[32] = {
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};

int
f24 (unsigned x)
{
  return (*(volatile const unsigned char (*)[32]) &c24)
	   [((x & -x) * 0x077CB531u) >> 27];
}

/* The strlen pass must not take the bytes that a volatile read loads from
   the initializer of the object.  */
typedef unsigned __attribute__ ((may_alias)) ua;
static const unsigned c25 = 0x6261;

__SIZE_TYPE__
f25 (void)
{
  char buf[8] __attribute__ ((aligned (4)));
  *(ua *) buf = *(volatile const unsigned *) &c25;
  buf[4] = 0;
  return __builtin_strlen (buf);
}

/* { dg-final { scan-assembler {:lo12:c24\M} } } */
/* { dg-final { scan-assembler {:lo12:c25\M} } } */
/* { dg-final { scan-assembler {\mstrlen\M} } } */
