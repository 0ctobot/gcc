/* { dg-do run } */
/* { dg-require-weak "" } */
/* { dg-additional-sources "inner-array-alias-8b.c" } */
/* { dg-additional-options "-Wno-lto-type-mismatch" } */

/* A variable index into the flexible array member of x, or into the [1]
   array arr before it, can reach elements past those of the initializer
   here.  x is a weak definition, and the strong definition in
   inner-array-alias-8b.c that replaces it at link time initializes more
   elements of flex, so the size of the weak definition doesn't bound what
   x.flex[i] and x.arr[i] can reach.  The two definitions differ in size
   on purpose, which LTO would otherwise warn about.  */

struct s { int a; int arr[1]; int b; int flex[]; };
struct s x __attribute__ ((weak)) = { 1, { 2 }, 3, { 4 } };

int __attribute__ ((noipa))
f (int i)
{
  x.flex[2] = 1;
  x.flex[i] = 2;
  return x.flex[2];
}

int __attribute__ ((noipa))
g (int i)
{
  int t = x.flex[1];
  x.arr[i] = 7;
  return x.flex[1] - t;
}

int
main (void)
{
  if (g (3) != 2)
    __builtin_abort ();
  if (f (2) != 2)
    __builtin_abort ();
  return 0;
}
