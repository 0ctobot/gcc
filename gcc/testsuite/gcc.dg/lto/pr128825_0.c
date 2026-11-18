/* PR ipa/128825 */
/* An indirect call that is passed the address of a member of a parameter,
   in a function inlined at WPA into a caller that passes a constant.  */
/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto } } } */

struct S { long pad; int y; };

extern void (*fp) (const int *);
extern void call (const struct S *);

static const struct S cs = { 1, 2 };
static int seen;

static void
check (const int *p)
{
  seen = *p;
}

int
main (void)
{
  fp = check;
  call (&cs);
  if (seen != 2)
    __builtin_abort ();
  return 0;
}
