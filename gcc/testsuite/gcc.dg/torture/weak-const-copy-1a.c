/* { dg-do compile } */

struct P { int x, y; };

static __SIZE_TYPE__
seven (const char *s)
{
  (void) s;
  return 7;
}

/* These replace the weak definitions in weak-const-copy-1.c.  */
const struct P wp = { 70, 80 };
__SIZE_TYPE__ (*const wl) (const char *) = seven;
