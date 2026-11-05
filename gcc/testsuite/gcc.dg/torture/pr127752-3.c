/* PR127752: An object with a volatile member that's never written must stay in
   its writable section and must not be merged with an identical object, since
   a linker script may map that section onto a device.  */
/* { dg-do compile { target aarch64*-*-* } } */
/* { dg-skip-if "" { *-*-* } { "-flto" } { "" } } */
/* { dg-additional-options "-g0 -fno-section-anchors -fno-inline" } */

struct S { volatile short a; int b; };

/* Read-only access keeps the initializer in the .data section.  */
static struct S c1 = { 1, 2 };
int f1 (void) { return c1.a; }

/* Without an initializer it stays in the .bss section.  */
static struct S c2;
int f2 (void) { return c2.a; }

/* A store-only object keeps its initializer.  */
static struct S c3 = { 3, 4 };
void f3 (void) { c3.a = 5; }

/* Identical objects stay separate.  */
static struct S c4 = { 5, 6 };
int f4 (void) { return c4.a; }
static struct S c5 = { 5, 6 };
int f5 (void) { return c5.a; }

/* Likewise, for objects without an initializer.  */
static struct S c6;
int f6 (void) { return c6.a; }
static struct S c7;
int f7 (void) { return c7.a; }

/* Likewise, for objects declared const, which stay read-only.  */
static const struct S c8 = { 7, 8 };
int f8 (void) { return c8.a; }
static const struct S c9 = { 7, 8 };
int f9 (void) { return c9.a; }

/* { dg-final { scan-assembler {\.data\n(?:(?!\s+\.(?:section|bss|text))[^\n]*\n)*c1:} } } */
/* { dg-final { scan-assembler {\.comm\s+c2,} } } */
/* { dg-final { scan-assembler {\.data\n(?:(?!\s+\.(?:section|bss|text))[^\n]*\n)*c3:} } } */
/* { dg-final { scan-assembler {:lo12:c4\M} } } */
/* { dg-final { scan-assembler {:lo12:c5\M} } } */
/* { dg-final { scan-assembler {:lo12:c6\M} } } */
/* { dg-final { scan-assembler {:lo12:c7\M} } } */
/* { dg-final { scan-assembler {:lo12:c8\M} } } */
/* { dg-final { scan-assembler {:lo12:c9\M} } } */
/* { dg-final { scan-assembler {\.section\s+\.rodata\n(?:(?!\s+\.(?:data|bss|text))[^\n]*\n)*c8:} } } */
/* { dg-final { scan-assembler {\.section\s+\.rodata\n(?:(?!\s+\.(?:data|bss|text))[^\n]*\n)*c9:} } } */
