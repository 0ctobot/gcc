// PR ipa/123405
// Any number of ungrouped variables in a section share one section object, so
// one directive covers them, even with a member of a COMDAT group in the same
// section.
// { dg-do compile { target c++17 } }
// { dg-require-named-sections "" }
// { dg-require-effective-target comdat_group }
// { dg-options "-O2" }

#define S __attribute__((section(".bss.foo"), used))

S static int v0;
S static int v1;
S static int v2;
S static int v3;

__attribute__((section(".bss.foo"))) inline int gv;

int *
use ()
{
  return &gv;
}

// { dg-final { scan-assembler-times {\.section[ \t]+\.bss\.foo,"aw"} 1 } }
// { dg-final { scan-assembler-times {\.section[ \t]+\.bss\.foo,"awG"} 1 } }
