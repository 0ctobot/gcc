/* The selective scheduler used to move a load into a register that was live
   on another path, and the clobbered value led to SIGFPE at run time.  */
/* { dg-do run } */
/* { dg-require-effective-target scheduling } */
/* { dg-require-effective-target fpic } */
/* { dg-options "-O3 -fsel-sched-pipelining -fselective-scheduling2 -fPIC" } */

int printf(const char *, ...);
int a, d, b, i = 2, e;
static unsigned c = 1;
char f, g = 1;
void j() {
k:
  if (b)
    for (;;)
      __builtin_abort();
  unsigned l = i && 2 / c * d % g;
  if (a) {
    l = c;
    g = i;
    if (d > 1) {
      printf("%d", c);
      goto k;
    }
  }
  a = c = l;
}
int main() {
  a = -2;
  j();
  i = 0;
  f = ~c;
  if (f > 0) {
    printf("%d", f);
    a = 2 % i;
  }
  return 0;
}
