// PR ipa/123405
// Retaining one of two functions in a section must keep the label and the body
// of each function together, otherwise the size expression of the other one
// spans two sections and doesn't assemble.
// { dg-do assemble { target R_flag_in_section } }
// { dg-require-named-sections "" }
// { dg-require-effective-target comdat_group }
// { dg-options "-O0" }

__attribute__((section(".mt"), retain)) inline int
f (int x)
{
  return x + 1;
}

__attribute__((section(".mt"))) inline int
g (int x)
{
  return x + 2;
}

__attribute__((section(".mu"))) inline int
h (int x)
{
  return x + 3;
}

__attribute__((section(".mu"), retain)) inline int
i (int x)
{
  return x + 4;
}

int
use (int x)
{
  return f (x) + g (x) + h (x) + i (x);
}
