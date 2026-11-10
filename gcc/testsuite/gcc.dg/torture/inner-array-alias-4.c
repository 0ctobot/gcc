/* A variable index into a [1] array that isn't the last member of its
   structure reaches the member that follows it, so data dependence analysis
   must not separate the two by their field offsets.  Each loop reads
   more[i + 1] before the store to e[i + 1] in the same iteration zeroes it,
   which loop distribution must not turn into a memset ahead of the loads.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -fno-strict-aliasing" } */

struct flat { int n; int e[1]; int more[64]; };
struct hdr1 { int n; int e[1]; };
struct box1 { struct hdr1 h; int more[64]; };
struct flat F;
struct box1 G;
int s[64];

__attribute__((noipa)) void
d_flat (int n)
{
  for (int i = 0; i < n; i++)
    {
      F.e[i + 1] = 0;		/* e[k + 1] is more[k].  */
      s[i] = F.more[i + 1];	/* Not zeroed yet in iteration i.  */
    }
}

__attribute__((noipa)) void
d_box (int n)
{
  for (int i = 0; i < n; i++)
    {
      G.h.e[i + 1] = 0;
      s[i] = G.more[i + 1];
    }
}

int
main (void)
{
  for (int i = 0; i < 64; i++)
    F.more[i] = G.more[i] = 100 + i;
  d_flat (40);
  if (s[5] != 106)
    __builtin_abort ();
  d_box (40);
  if (s[5] != 106)
    __builtin_abort ();
  return 0;
}
