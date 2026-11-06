/* PR middle-end/128312 */
/* { dg-do run } */

/* A variable keeps its old value when a jump back runs its initializer again,
   so the call in the initializer must read the old member through the pointer.
   This covers the jumps that find_back_jump_labels has to recognize.  */

struct T { int k; int pad0; long a[6]; };

__attribute__ ((noipa)) long
g (long x)
{
  return x;
}

__attribute__ ((noipa)) long
plain (long n)
{
  struct T *p = 0;
again:;
  struct T v = (struct T) { .k = n + 1, .a[3] = g (p ? p->k : -1) };
  p = &v;
  if (n++ < 1)
    goto again;
  return v.a[3];
}

/* walk_tree visits the initializer of w ahead of the label.  */

__attribute__ ((noipa)) long
in_later_init (long n)
{
  struct T *p = 0;
again:;
  struct T v = (struct T) { .k = n + 1, .a[3] = g (p ? p->k : -1) };
  p = &v;
  long w = ({ if (n++ < 1) goto again; 0; });
  return v.a[3] + w;
}

__attribute__ ((noipa)) long
in_inner_block (long n)
{
  struct T *p = 0;
  { again:; g (0); }
  struct T v = (struct T) { .k = n + 1, .a[3] = g (p ? p->k : -1) };
  p = &v;
  for (int i = 0; i < 2; i++)
    if (n++ < 1)
      goto again;
  return v.a[3];
}

__attribute__ ((noipa)) long
computed (long n)
{
  struct T *p = 0;
  void *l = &&again;
again:;
  struct T v = (struct T) { .k = n + 1, .a[3] = g (p ? p->k : -1) };
  p = &v;
  if (n++ < 1)
    goto *l;
  return v.a[3];
}

__attribute__ ((noipa)) long
in_nested (long n)
{
  __label__ again;
  struct T *p = 0;
  __attribute__ ((noinline)) void hop (void) { goto again; }
again:;
  struct T v = (struct T) { .k = n + 1, .a[3] = g (p ? p->k : -1) };
  p = &v;
  if (n++ < 1)
    hop ();
  return v.a[3];
}

#if defined (__aarch64__) || defined (__x86_64__)
__attribute__ ((noipa)) long
asm_goto (long n)
{
  struct T *p = 0;
  int back;
again:;
  struct T v = (struct T) { .k = n + 1, .a[3] = g (p ? p->k : -1) };
  p = &v;
  back = n++ < 1;
#ifdef __aarch64__
  __asm__ goto ("cbnz %w0, %l1" : : "r" (back) : : again);
#else
  __asm__ goto ("testl %0, %0\n\tjnz %l1" : : "r" (back) : "cc" : again);
#endif
  return v.a[3];
}
#endif

int
main (void)
{
  if (plain (0) != 1)
    __builtin_abort ();
  if (in_later_init (0) != 1)
    __builtin_abort ();
  if (in_inner_block (0) != 1)
    __builtin_abort ();
  if (computed (0) != 1)
    __builtin_abort ();
  if (in_nested (0) != 1)
    __builtin_abort ();
#if defined (__aarch64__) || defined (__x86_64__)
  if (asm_goto (0) != 1)
    __builtin_abort ();
#endif
  return 0;
}
