/* The store's address is an integer sum of a local's address and an offset
   that takes it out of the local, the way boost::interprocess::offset_ptr
   works.  IVOPTS must not base the reference on the local, since the store
   would then look dead and get deleted once the local dies after the loop.  */
/* { dg-do run } */
/* { dg-options "-Os -fdump-tree-ivopts" } */

typedef __UINTPTR_TYPE__ uintptr_t;

struct it
{
  uintptr_t off;
};

unsigned long sum;

__attribute__ ((noipa)) void
fill (int *p, long step, long n, int sel)
{
  {
    struct it cur;
    uintptr_t off = (uintptr_t) p - (uintptr_t) &cur;
    if (sel)
      off += step;
    uintptr_t end = off + n * step;
    cur.off = off;
    do
      {
	*(int *) ((uintptr_t) &cur + cur.off) = 1;
	sum += (unsigned long) cur.off * 3;
	cur.off += step;
      }
    while (cur.off != end);
  }
}

int a[16];

int
main (void)
{
  fill (a, sizeof (int), 4, 1);
  for (int i = 0; i < 16; i++)
    if (a[i] != (i >= 1 && i <= 4))
      __builtin_abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-not "MEM\\\[\\(int \\*\\)&cur" "ivopts" } } */
