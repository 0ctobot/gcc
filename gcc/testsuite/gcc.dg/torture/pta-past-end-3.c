/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* The address one past the end of a member array, or the address of a
   zero-length array, moved back by one element gives a pointer to the
   member before it.  Points-to analysis must represent the address by a
   member that a pointer moved back from it can reach, or the loads below
   are computed to read the wrong member and the store through x is
   missed.  */

/* The array is the last member, so the address is one past the end of the
   struct.  */

struct arr1 { int *n; int *items[2]; };

__attribute__((noipa)) int
f1 (int c)
{
  int l1 = 0, l2 = 0;
  struct arr1 s = { &l2, { &l2, &l1 } }, t = { &l2, { &l2, &l1 } };
  int **p = c ? &s.items[2] : &t.items[2];
  int *x = p[-1];
  l1 = 1;
  *x = 2;
  return l1;
}

/* The array is followed by padding, so the address is in that padding.  */

struct arr2 { int *items[1]; _Alignas (16) int *z; };

__attribute__((noipa)) int
f2 (int c)
{
  int l1 = 0, l2 = 0;
  struct arr2 s = { { &l1 }, &l2 }, t = { { &l1 }, &l2 };
  int **p = c ? &s.items[1] : &t.items[1];
  int *x = p[-1];
  l1 = 1;
  *x = 2;
  return l1;
}

/* A zero-length array at the end of a struct.  */

struct flex { int *a; int *b; int *data[0]; };

__attribute__((noipa)) int
f3 (int c)
{
  int l1 = 0, l2 = 0;
  struct flex s = { &l2, &l1 }, t = { &l2, &l1 };
  int **p = c ? s.data : t.data;
  int *x = p[-1];
  l1 = 1;
  *x = 2;
  return l1;
}

int
main (void)
{
  if (f1 (1) != 2 || f1 (0) != 2)
    __builtin_abort ();
  if (f2 (1) != 2 || f2 (0) != 2)
    __builtin_abort ();
  if (f3 (1) != 2 || f3 (0) != 2)
    __builtin_abort ();
  return 0;
}
