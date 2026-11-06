// PR ipa/123405
// A member of a COMDAT group can share a section name with ungrouped data, in
// either order, including when the ungrouped data or a jump table of the group
// is in a read-only section.
// { dg-do compile { target c++17 } }
// { dg-require-named-sections "" }
// { dg-require-effective-target comdat_group }
// { dg-options "-O2" }

__attribute__((section(".foo"))) inline int a = 1;
__attribute__((section(".foo"))) int b = 2;

__attribute__((section(".bar"))) int c = 3;
__attribute__((section(".bar"))) inline int d = 4;

__attribute__((section(".rodata.baz"))) extern const int k = 5;

__attribute__((section(".baz"), noinline)) inline int
f (int x)
{
  switch (x)
    {
    case 0: return a * 2;
    case 1: return b * 3;
    case 2: return c * 5;
    case 3: return d * 7;
    case 4: return a * 11;
    case 5: return b * 13;
    case 6: return c * 17;
    case 7: return d * 19;
    case 8: return a * 23;
    case 9: return b * 29;
    default: return k;
    }
}

int
g (int x)
{
  return f (x) + a + d;
}

// { dg-final { scan-assembler {\.section[ \t]+\.foo,"awG",[@%]progbits,_?a,comdat} } }
// { dg-final { scan-assembler {\.section[ \t]+\.foo,"aw"(,[@%]progbits)?\n} } }
// { dg-final { scan-assembler {\.section[ \t]+\.bar,"awG",[@%]progbits,_?d,comdat} } }
// { dg-final { scan-assembler {\.section[ \t]+\.bar,"aw"(,[@%]progbits)?\n} } }
