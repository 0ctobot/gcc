// PR ipa/123405
// Code and data with one section name still conflict across COMDAT groups.
// { dg-do compile { target c++17 } }
// { dg-require-named-sections "" }
// { dg-require-effective-target comdat_group }
// { dg-options "-O2" }

__attribute__((section(".foo"))) inline int
f ()
{
  return 1;
}

__attribute__((section(".foo"))) inline int d = 2; // { dg-error "section type conflict" }

int (*pf) () = f;
int *pd = &d;
