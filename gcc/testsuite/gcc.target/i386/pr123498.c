/* PR target/123498 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O" } */

union {
  int w;
  __bf16 f;
} u;

int a, c;

void
foo (void)
{
  int x = x;
  do {
    __builtin_sub_overflow (u.w, 1, &c);
    a %= u.w = x;
    u.f--;
  } while (c);
}

/* { dg-final { scan-assembler "pushl\[ \t\]+\\\$0x0000" } } */
