// PR tree-optimization/127874
// { dg-do compile }

typedef __SIZE_TYPE__ size_t;
extern "C" {
extern void *__builtin_memset (void *, int, size_t) __attribute__((noreturn));
int g;
__attribute__((noipa)) void
f (char *p)
{
  *p = 0;
  if (g)
    __builtin_memset (p, 0, 1);
}
}
