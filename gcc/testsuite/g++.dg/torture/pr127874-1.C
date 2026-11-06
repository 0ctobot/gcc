// PR tree-optimization/127874
// { dg-do run }

typedef __SIZE_TYPE__ size_t;
extern "C" void *__builtin_memset (void *, int, size_t) __attribute__((noreturn));
extern "C" void *calloc (size_t, size_t);
int g = 1;
extern "C" void
h (void)
{
  __builtin_abort ();
}
void *
f (size_t n)
{
  char *p = (char *) calloc (n, 1);
  if (g)
    __builtin_memset (p, 0, n);
  else
    h ();
  return p;
}
int
main ()
{
  if (!f (16))
    __builtin_abort ();
}
