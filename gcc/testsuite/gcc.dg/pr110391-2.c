/* PR rtl-optimization/110391 */
/* The selective scheduler used to miscompile this program into an infinite
   loop.  */
/* { dg-do run } */
/* { dg-require-effective-target scheduling } */
/* { dg-options "-O2 -fselective-scheduling2" } */

int printf(const char *, ...);
int a = -1, b = 1, *c = &a;
int main() {
  *c |= 1;
  if (a) {
  L:
    if (a < -1)
      printf("%d", 0);
    if (!b) {
      printf("%d", 1);
      goto L;
    }
    if (a > 0)
      goto L;
    b = 0;
  }
  return 0;
}
