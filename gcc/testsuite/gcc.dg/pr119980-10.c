/* PR target/119980 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* S has a char array, so an access through it has base alias set zero and
   only its own alias set disambiguates it.  The copy of struct F into s->f
   doesn't conflict with the load of struct B through Q, but the int store
   does, so the int store has to stay even though it writes the value that the
   copy just put there.  */

struct F { int x; int y[7]; };
struct B { int x; };
struct S { struct F f; unsigned char pad[4]; };
struct B out;

__attribute__((noipa)) void
copy (struct S *s, struct F *src, struct B *q)
{
  q->x = 1;
  int v = src->x;
  s->f = *src;
  *(int *) &s->f = v;
  out = *q;
}

int
main (void)
{
  void *m = __builtin_malloc (sizeof (struct S));
  struct F *src = __builtin_calloc (1, sizeof (struct F));
  if (!m || !src)
    return 0;
  src->x = 2;
  copy (m, src, m);
  if (out.x != 2)
    __builtin_abort ();
  return 0;
}
