/* { dg-options "-fno-lto" } */

extern void pub (void);

int sel;

int
ext (int x)
{
  if (x == 12345)
    pub ();
  return x;
}
