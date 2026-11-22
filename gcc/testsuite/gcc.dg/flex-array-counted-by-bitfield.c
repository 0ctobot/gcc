/* { dg-do run } */
/* { dg-options "-O2" } */

struct bits
{
  unsigned int first : 3;
  unsigned int count : 5;
  int data[] __attribute__ ((counted_by (count)));
};

struct nested
{
  struct { unsigned int count : __INT_WIDTH__; };
  int *data __attribute__ ((counted_by (count)));
};

struct plain
{
  unsigned int count;
  int *data __attribute__ ((counted_by (count)));
};

int
main (void)
{
  struct bits a = { 1, 2 };
  struct nested b = { { 0 }, 0 };
  struct plain c = { 0, 0 };

  if (__builtin_counted_by_ref (a.data) != 0
      || __builtin_counted_by_ref (b.data) != 0
      || __builtin_counted_by_ref (c.data) != &c.count)
    __builtin_abort ();
  return 0;
}
