/* PR sanitizer/120950 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O2 -fsanitize=address -mpreferred-stack-boundary=2" } */

struct T {
  unsigned short a, b;
  unsigned int c;
};

void g (struct T *);

void
f (int x)
{
  struct T value;
  value.c = x;
  g (&value);
}

/* The shadow bytes for the frame are only right if its base is aligned to a
   shadow granule.  */
/* { dg-final { scan-assembler "andl\[ \t\]+\\\$-8, %esp" } } */
