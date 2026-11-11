/* { dg-options "-mtp=tpidrro_el0" } */

extern __thread int w[4];

struct ref { int *p; };
struct two { int *p; int k; };

int *volatile sink;

__attribute__ ((noinline)) int *
step (int *p, int k)
{
  sink = p;
  for (int i = 0; i < k; i++)
    sink = sink + 1;
  return sink;
}

__attribute__ ((noinline)) int *
step_ref (const struct ref *r, int k)
{
  sink = r->p;
  for (int i = 0; i < k; i++)
    sink = sink + 1;
  return sink;
}

__attribute__ ((noinline)) int *
step_two (struct two t)
{
  sink = t.p;
  for (int i = 0; i < t.k; i++)
    sink = sink + 1;
  return sink;
}

__attribute__ ((noinline)) int *
ro_step (int k)
{
  return step (w, k);
}

__attribute__ ((noipa)) int *
ro_ref (int k)
{
  return &w[k];
}
