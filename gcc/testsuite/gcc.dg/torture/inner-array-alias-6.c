/* A [1] array at the end of the last element of an array of structures can
   run past that array into the members after it.  The range of the outer
   index then doesn't bound the access, so a store through the inner array
   must not be taken to leave those members alone.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -fno-strict-aliasing" } */

struct hdr1 { int n; int ids[1]; };
struct cmd { int n; struct hdr1 h[2]; int a; int b; int c; };

__attribute__((noipa)) int
past_b (struct cmd *p, unsigned int j, int i)
{
  int x = p->b;
  p->h[j & 1].ids[i] = 2;	/* j == 1, i == 2 is b.  */
  return x + p->b;
}

__attribute__((noipa)) int
past_c (struct cmd *p, unsigned int j, int i)
{
  int x = p->c;
  if (j > 1)
    return -1;
  p->h[j].ids[i] = 2;		/* j == 1, i == 3 is c.  */
  return x + p->c;
}

int
main (void)
{
  struct cmd c1 = { .b = 1 };
  struct cmd c2 = { .c = 1 };

  if (past_b (&c1, 1, 2) != 3 || past_c (&c2, 1, 3) != 3)
    __builtin_abort ();
  return 0;
}
