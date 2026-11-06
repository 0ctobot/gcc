/* PR target/104613 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O2 -fno-omit-frame-pointer" } */

struct big { char c[70000]; };

__attribute__ ((stdcall)) void
f (struct big b, long o)
{
  __builtin_eh_return (o, 0);
}
