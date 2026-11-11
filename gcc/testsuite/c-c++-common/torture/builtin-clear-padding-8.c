/* { dg-do run } */
/* { dg-require-effective-target size20plus } */

/* __builtin_clear_padding on a struct with a bit-field that follows more
   padding bytes than the buffer it computes the padding in holds.  */

struct S
{
  char a;
  unsigned long long b : 63 __attribute__((aligned (32768)));
  unsigned int c : 5;
};

struct T
{
  unsigned int a : 3;
  unsigned long long b : 64 __attribute__((aligned (8192)));
};

__attribute__((noipa)) void
foo (struct S *s)
{
  s->a = -1; s->b = 0x7fffffffffffffffULL; s->c = 31;
}

__attribute__((noipa)) void
bar (struct T *t)
{
  t->a = 7; t->b = -1;
}

int
main ()
{
  struct S s1, s2;
  struct T t1, t2;
  __builtin_memset (&s1, 0, sizeof (s1));
  __builtin_memset (&s2, -1, sizeof (s2));
  foo (&s1);
  foo (&s2);
  __builtin_clear_padding (&s2);
  if (__builtin_memcmp (&s1, &s2, sizeof (s1)))
    __builtin_abort ();
  __builtin_memset (&t1, 0, sizeof (t1));
  __builtin_memset (&t2, -1, sizeof (t2));
  bar (&t1);
  bar (&t2);
  __builtin_clear_padding (&t2);
  if (__builtin_memcmp (&t1, &t2, sizeof (t1)))
    __builtin_abort ();
  return 0;
}
