/* A variable index into a [1] array reaches the members after it, so each
   iteration of the loop below reads what the previous one stored: e[i + 5] of
   g[i] is the int at byte 20 * (i + 1) of g, which is e[i + 1] of g[i + 1].
   The vectorizer's run-time alias check mustn't compare only the indices into
   e, since g[i].e moves with i.  */
/* { dg-do run } */
/* { dg-additional-options "-ftree-vectorize -fno-vect-cost-model" } */

struct S { int e[1]; int more[3]; };
struct S g[64];

__attribute__((noipa)) void
f (long n, long m)
{
  for (long i = 0; i < n; i++)
    g[i].e[i + m] = g[i].e[i] + 1;
}

__attribute__((noipa)) int
get (long k)
{
  return ((int *) g)[k];
}

int
main (void)
{
  g[0].e[0] = 7;
  f (32, 5);
  for (long k = 0; k <= 32; k++)
    if (get (5 * k) != 7 + k)
      __builtin_abort ();
  return 0;
}
