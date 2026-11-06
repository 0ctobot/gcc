// PR ipa/123405
// A constant variable and a variable that's writable only because of its
// relocation each stay in their own COMDAT group.
// { dg-do compile { target { c++17 && fpic } } }
// { dg-require-named-sections "" }
// { dg-require-effective-target comdat_group }
// { dg-options "-O2 -fPIC" }

int t;

__attribute__((section(".mysec"))) inline const int a = 1;
__attribute__((section(".mysec"))) inline int *const b = &t;

const int *
pa ()
{
  return &a;
}

int *const *
pb ()
{
  return &b;
}

// { dg-final { scan-assembler {\.section[ \t]+\.mysec,"aG",[@%]progbits,_?a,comdat} } }
// { dg-final { scan-assembler {\.section[ \t]+\.mysec,"awG",[@%]progbits,_?b,comdat} } }
