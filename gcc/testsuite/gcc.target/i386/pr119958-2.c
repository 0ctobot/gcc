/* PR target/119958 */
/* { dg-do compile { target { fpic && *-*-linux* } } } */
/* { dg-options "-O2 -fpic -ftls-model=initial-exec" } */

extern __thread int f __attribute__ ((weak));
extern __thread int hh;
extern __thread int b;
int h (int);
int h1 (int *);

int
g (void)
{
  h1 (&hh);
  if (&f)
    return h1 (&f);
  return 0;
}

int
s (void)
{
  h1 (&hh);
  if ((long) &b < 0)
    return h (1);
  return 0;
}

/* The linker can relax the add to an lea, so no branch can use its flags.  */
/* { dg-final { scan-assembler-not {@(gottpoff|gotntpoff|indntpoff)[^\n]*\n\tj[a-z]+} } } */
