// The C++ counterparts of the [0] cases in gcc.dg/torture/inner-array-alias-*.c.
// A [0] array that isn't the last member overlaps the members after it:
// M.mark[1] is M.more[1], and B.h.data[1] is B.storage[1].  The C++ front end
// represents such an array differently from C, with the bounds [0, SIZE_MAX].
// { dg-do run }
// { dg-additional-options "-fno-strict-aliasing" }

struct mk { int n; int mark[0]; int more[64]; };
struct hdr { int n; int data[0]; };
struct box { struct hdr h; int storage[64]; };
struct mk M;
struct box B;

__attribute__((noipa)) int
g_zero (long i, long j)
{
  M.more[i] = 1;
  M.mark[j] = 2;
  return M.more[i];
}

__attribute__((noipa)) int
g_box (long i, long j)
{
  B.storage[i] = 1;
  B.h.data[j] = 2;
  return B.storage[i];
}

__attribute__((noipa)) int
g_zero_const ()
{
  M.more[1] = 1;
  M.mark[1] = 2;
  return M.more[1];
}

__attribute__((noipa)) int
g_box_const ()
{
  B.storage[1] = 1;
  B.h.data[1] = 2;
  return B.storage[1];
}

int
main ()
{
  if (g_zero (1, 1) != 2)
    __builtin_abort ();
  if (g_box (1, 1) != 2)
    __builtin_abort ();
  if (g_zero_const () != 2)
    __builtin_abort ();
  if (g_box_const () != 2)
    __builtin_abort ();
  return 0;
}
