/* A compound literal with static storage keeps the incomplete array type of its
   flexible array data, which mustn't take the size of that data for -1 when the
   literal is emitted.  That puts zeros after the literal which the layout of
   its section doesn't count, and the objects after it end up in the wrong place
   for code that reaches them through a section anchor.  */
/* { dg-do run } */
/* { dg-options "-std=gnu23 -O2" } */

struct F { int a; int f[]; };
struct S { int a; char s[]; };

int before = 9;

__attribute__ ((noipa)) struct F *
get_two (void)
{
  return &(static struct F) { 4, { 5, 6 } };
}

__attribute__ ((noipa)) struct S *
get_string (void)
{
  return &(static struct S) { 7, "abc" };
}

__attribute__ ((noipa)) struct F *
get_empty (void)
{
  return &(static struct F) { 8, {} };
}

int after = 10;

__attribute__ ((noipa)) int
read_after (void)
{
  return after + before;
}

int
main (void)
{
  struct F *p = get_two ();
  struct S *q = get_string ();
  struct F *r = get_empty ();

  if (p->a != 4 || p->f[0] != 5 || p->f[1] != 6)
    __builtin_abort ();
  if (q->a != 7 || __builtin_strcmp (q->s, "abc"))
    __builtin_abort ();
  if (r->a != 8)
    __builtin_abort ();
  if (read_after () != 19)
    __builtin_abort ();
  return 0;
}
