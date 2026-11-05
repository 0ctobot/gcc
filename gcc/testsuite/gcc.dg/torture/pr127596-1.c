/* PR middle-end/127596 */
/* { dg-do run { target lp64 } } */
/* { dg-additional-options "-fno-thread-jumps -ftracer -fno-tree-forwprop" } */

/* The address of x only becomes live through a degenerate PHI in a block with
   a single predecessor, while v is already live coming into that block.  x and
   v have to conflict since x ends up holding the address of v.  */

short d = -7, k = -2586, l, n = -8, p = -1, q = -17829;
int m = -15, o, r;
int a(int s, int u) {
  int e[256];
  for (int f = 0; f < 256; ++f) {
    unsigned c = f;
    for (int g = 0; g < 8; ++g)
      c = c & 1 ? c >> 1 ^ 3988292384 : c >> 1;
    e[f] = c;
  }
  unsigned h = s;
  int i = u;
  for (int b = 0; b < 4; ++b) {
    char ab = i;
    i >>= 8;
    h = h >> 8 ^ e[(h ^ ab) & 255];
  }
  return h;
}
void j(int s, int u) {
  if (s != u)
    __builtin_abort();
}
int t(char s, long u) {
  int v = -244, w, *x, **ak;
  x = &w;
  ak = &x;
  o = -524300;
  do {
    *x = 2146435073 - u;
    if (~w != -3) {
      do {
        ak = &x + 2;
        if (w)
          goto ag;
        *x = 0;
      } while (0 < u - s);
    }
    if (!l) {
    ag:
      *ak = &v;
      w = -2147483628;
    }
} while (o < w);
  r = a(0, m);
  r = a(r, 12);
  r = a(r, (long)((unsigned long)k << 48) >> 48);
  r = a(r, (long)((unsigned long)d << 48) >> 48);
  r = a(r, 0);
  r = a(r, (long)((unsigned long)32768 << 48) >> 48);
  r = a(r, v);
  r = a(r, 1576633);
  r = a(r, 39);
  r = a(r, (long)((unsigned long)o << 40) >> 40);
  r = a(r, 27);
  r = a(r, 11085);
  r = a(r, 19737);
  r = a(r, (long)((unsigned long)q << 48) >> 48);
  r = a(r, p);
  r = a(r, (long)((unsigned long)n << 48) >> 48);
  r = a(r, 18546);
  r = a(r, 235);
  r = a(r, w);
  r = a(r, 0);
  r = a(r, u);
  r = a(r, 0);
  r = a(r, 0);
  r = a(r, 0);
  r = a(r, *x);
  r = a(r, 19737);
  return r;
}
int main() {
  int aj = t(0, 2146435071);
  j(-965382002, aj);
  return 0;
}
