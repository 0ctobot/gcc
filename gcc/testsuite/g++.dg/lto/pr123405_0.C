// { dg-lto-do run }
// { dg-require-named-sections "" }
// { dg-lto-options { { -O2 -flto } } }

// Localizing a COMDAT function during LTO may only drop a section the compiler
// picked itself.  The user asked for these two functions to live in
// root_section, and both must stay there.

template <int N>
struct S
{
  [[gnu::section ("root_section"), gnu::noinline, gnu::used]]
  static int a (int x) { return x + N; }
  [[gnu::section ("root_section"), gnu::noinline]]
  static int b (int x) { return x * N; }
};

extern char __start_root_section[], __stop_root_section[];

int (*volatile pa) (int) = S<3>::a;
int (*volatile pb) (int) = S<3>::b;

int
main ()
{
  char *p = (char *) pa, *q = (char *) pb;
  if (p < __start_root_section || p >= __stop_root_section)
    __builtin_abort ();
  if (q < __start_root_section || q >= __stop_root_section)
    __builtin_abort ();
  return pa (1) + pb (1) != 7;
}
