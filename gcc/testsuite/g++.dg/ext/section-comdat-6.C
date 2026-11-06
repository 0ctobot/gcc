// PR ipa/123405
// A retained variable and a member of another COMDAT group in one section stay
// in their own groups.  If the retained one lands in the other group, the
// linker discards it when this file's copy of that group wins.
// { dg-do run { target { c++17 && R_flag_in_section } } }
// { dg-require-named-sections "" }
// { dg-require-effective-target comdat_group }
// { dg-additional-sources "section-comdat-6a.cc" }
// { dg-options "-O2" }

__attribute__((section(".foo"))) inline int b = 2;

int *pa ();
int *pb ();

int
main ()
{
  return *pa () == 1 && *pb () == 2 && pb () == &b ? 0 : 1;
}
