/* PR tree-optimization/127874 */
/* { dg-do compile } */

/* IPA finds that this definition never returns, which must not mark the decl
   the compiler uses for its own memset calls as noreturn.  */

__attribute__((noinline)) void *
__builtin_memset (void *p, int c, __SIZE_TYPE__ n)
{
  for (;;)
    __asm__ ("");
}

int g;

void
f (char *p)
{
  *p = 0;
  if (g)
    __builtin_memset (p, 0, 1);
}
