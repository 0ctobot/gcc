// PR ipa/123405
// The linker keeps the COMDAT group of f from this file and discards the one
// from section-comdat-2a.cc, and g over there must not go away along with it.
// { dg-do run }
// { dg-require-named-sections "" }
// { dg-require-effective-target comdat_group }
// { dg-additional-sources "section-comdat-2a.cc" }
// { dg-options "-O2" }

__attribute__ ((section (".foo"), noinline)) inline int
f (int x)
{
  __asm__ ("");
  return x + 1;
}

int caller (int);

int
main ()
{
  if (f (1) != 2 || caller (1) != 5)
    __builtin_abort ();
}
