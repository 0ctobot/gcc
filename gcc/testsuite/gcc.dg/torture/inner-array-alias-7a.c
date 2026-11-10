/* { dg-do run } */
/* { dg-additional-sources "inner-array-alias-7b.c" } */

/* A variable index into the [1] array arr can reach the members after it,
   including the elements of the flexible array member.  x is only declared
   here, and its definition in inner-array-alias-7b.c initializes elements
   of flex that the size of the declared type doesn't include, so that size
   doesn't bound what x.arr[i] can reach.  */

struct s { int a; int arr[1]; int b; int flex[]; };
extern struct s x;

int __attribute__ ((noipa))
f (int i)
{
  int t = x.flex[1];
  x.arr[i] = 7;
  return x.flex[1] - t;
}

int
main (void)
{
  if (f (3) != 2)
    __builtin_abort ();
  return 0;
}
