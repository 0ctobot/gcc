/* A constant index past the end of a flexible, [1] or [0] array that isn't
   the last member reaches the member that follows, so a store through it
   must not be moved past or forwarded around an access to that member.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -fno-strict-aliasing" } */

struct hdr { int n; int data[]; };
struct box { struct hdr h; int storage[64]; };
struct flat { int n; int e[1]; int more[64]; };
struct mk { int n; int mark[0]; int more[64]; };
struct box B;
struct flat F;
struct mk M;

__attribute__((noipa)) int g_flex (long i)
{ B.storage[i] = 1; B.h.data[1] = 2; return B.storage[i]; }
__attribute__((noipa)) int g_one (long i)
{ F.more[i] = 1; F.e[2] = 2; return F.more[i]; }
__attribute__((noipa)) int g_zero (long i)
{ M.more[i] = 1; M.mark[1] = 2; return M.more[i]; }
__attribute__((noipa)) int p_flex (struct box *b, long i)
{ b->storage[i] = 1; b->h.data[1] = 2; return b->storage[i]; }
__attribute__((noipa)) int p_one (struct flat *f, long i)
{ f->more[i] = 1; f->e[2] = 2; return f->more[i]; }
__attribute__((noipa)) int p_zero (struct mk *m, long i)
{ m->more[i] = 1; m->mark[1] = 2; return m->more[i]; }

int
main (void)
{
  if (g_flex (1) != 2 || g_one (1) != 2 || g_zero (1) != 2)
    __builtin_abort ();
  if (p_flex (&B, 1) != 2 || p_one (&F, 1) != 2 || p_zero (&M, 1) != 2)
    __builtin_abort ();
  return 0;
}
