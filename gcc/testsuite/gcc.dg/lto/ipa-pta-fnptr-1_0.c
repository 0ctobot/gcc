/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto -flto-partition=1to1 -fipa-pta } } } */

/* IPA-PTA runs once per partition, so ext1 and ext2 have no function info in
   the partition holding h. Calling h must still count as clobbering g.  */

extern void abort (void);
extern void ext1 (void);
extern void ext2 (void);

int g;
volatile int sel;

__attribute__ ((noinline)) static void
h (void (*fp) (void))
{
  fp ();
}

int
main (void)
{
  g = 1;
  h (sel ? ext1 : ext2);
  if (g != 3)
    abort ();
  sel = 1;
  g = 1;
  h (sel ? ext1 : ext2);
  if (g != 2)
    abort ();
  return 0;
}
