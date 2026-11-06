// PR ipa/123405
// Inline variables keep their COMDAT groups next to a variable that
// -fwhole-program takes out of its group.
// { dg-do compile { target c++17 } }
// { dg-require-named-sections "" }
// { dg-require-effective-target comdat_group }
// { dg-options "-O2 -fwhole-program" }

__attribute__((section(".foo"))) inline int a = 1;
__attribute__((section(".foo"), externally_visible)) inline int b = 2;

__attribute__((section(".bar"), externally_visible)) inline int p = 3;
__attribute__((section(".bar"), externally_visible)) inline int q = 4;

int
main ()
{
  return a + b + p + q;
}

// { dg-final { scan-assembler {\.section[ \t]+\.foo,"awG",[@%]progbits,_?b,comdat} } }
// { dg-final { scan-assembler {\.section[ \t]+\.bar,"awG",[@%]progbits,_?p,comdat} } }
// { dg-final { scan-assembler {\.section[ \t]+\.bar,"awG",[@%]progbits,_?q,comdat} } }
