/* Unroll-and-jam must not fuse inner loops that contain volatile accesses,
   since that interleaves the accesses of consecutive outer iterations.  */
/* { dg-do compile } */
/* { dg-options "-O3 -fdump-tree-unrolljam-details" } */
/* { dg-require-effective-target int32plus } */

typedef unsigned int u32;
#define WRITE_ONCE(x, v) do { *(volatile __typeof__ (x) *) &(x) = (v); } while (0)
extern u32 x[65][64], y[64][64];
extern volatile u32 reg;
extern u32 preg;

/* The source does all 64 stores of i before any store of i + 1.  */
void
jam_write (int n)
{
  for (int i = 0; i < n; i++)
    for (int j = 0; j < 64; j++)
      {
	y[i][j] = x[i][j] + x[i + 1][j];
	WRITE_ONCE (preg, i);
      }
}

/* The k-th read of reg belongs to y[k / 64][k % 64].  */
void
jam_read (int n)
{
  for (int i = 0; i < n; i++)
    for (int j = 0; j < 64; j++)
      y[i][j] = x[i][j] + x[i + 1][j] + reg;
}

/* Without volatile accesses the nest is still unrolled and jammed.  */
void
jam_plain (int n)
{
  for (int i = 0; i < n; i++)
    for (int j = 0; j < 64; j++)
      {
	y[i][j] = x[i][j] + x[i + 1][j];
	preg = i;
      }
}

/* { dg-final { scan-tree-dump-times "applying unroll and jam" 1 "unrolljam" } } */
/* { dg-final { scan-tree-dump-times "Loop nest has volatile accesses" 2 "unrolljam" } } */
