// PR ipa/123405
// -fwhole-program localizes f, which stays in its user section, while g and h
// stay in COMDAT groups of their own in a section with the same name.
// { dg-do compile }
// { dg-require-named-sections "" }
// { dg-require-effective-target comdat_group }
// { dg-options "-O2 -fwhole-program" }

__attribute__ ((section (".foo"), noinline)) inline int
f (int x)
{
  __asm__ ("");
  return x + 1;
}

__attribute__ ((section (".foo"), noinline, externally_visible)) inline int
g (int x)
{
  __asm__ ("");
  return x + 2;
}

__attribute__ ((section (".foo"), noinline, externally_visible)) inline int
h (int x)
{
  __asm__ ("");
  return x + 3;
}

int
main (int argc, char **)
{
  return f (argc) + g (argc) + h (argc);
}

// { dg-final { scan-assembler {\.section\s+\.foo,"ax",} } }
// { dg-final { scan-assembler {\.section\s+\.foo,"axG",[^\n]*_Z1gi,comdat} } }
// { dg-final { scan-assembler {\.section\s+\.foo,"axG",[^\n]*_Z1hi,comdat} } }
