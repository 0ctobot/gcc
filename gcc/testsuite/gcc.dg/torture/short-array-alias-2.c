/* Each loop copies element I of the member that follows a flexible, [1] or
   [0] array and then stores 7 to element 1 of that member through a
   constant index past the end of the array.  The copy of element 1, made
   in the second iteration, must see the store, so neither loop distribution
   nor store motion may treat the two references as independent.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -fno-strict-aliasing" } */

struct hdr { int n; int data[]; };
struct box { struct hdr h; int storage[64]; };
struct flat { int n; int e[1]; int more[64]; };
struct mk { int n; int mark[0]; int more[64]; };
struct box B;
struct flat F;
struct mk M;
int s[3][64];

__attribute__((noipa)) void
d_flex (int n)
{
  for (int i = 0; i < n; i++)
    {
      s[0][i] = B.storage[i];
      B.h.data[1] = 7;
    }
}

__attribute__((noipa)) void
d_one (int n)
{
  for (int i = 0; i < n; i++)
    {
      s[1][i] = F.more[i];
      F.e[2] = 7;
    }
}

__attribute__((noipa)) void
d_zero (int n)
{
  for (int i = 0; i < n; i++)
    {
      s[2][i] = M.more[i];
      M.mark[1] = 7;
    }
}

int
main (void)
{
  for (int k = 0; k < 64; k++)
    B.storage[k] = F.more[k] = M.more[k] = 100 + k;
  d_flex (64);
  d_one (64);
  d_zero (64);
  if (s[0][1] != 7 || s[1][1] != 7 || s[2][1] != 7)
    __builtin_abort ();
  return 0;
}
