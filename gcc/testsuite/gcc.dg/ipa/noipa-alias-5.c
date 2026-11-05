/* PR ipa/127509 */
/* With -fPIC the weakref can't become a static alias since b doesn't bind
   locally, so it becomes a transparent alias of b instead.  A transparent alias
   inherits its availability from the end of the chain and skips over b, which
   mustn't hide the noipa on b.  */
/* { dg-do compile } */
/* { dg-require-alias "" } */
/* { dg-require-weak "" } */
/* { dg-require-effective-target fpic } */
/* { dg-options "-O2 -fPIC -fno-semantic-interposition -fdump-ipa-visibility -fdump-tree-optimized" } */

int g;

int a (int v) { g = v; return v; }
extern int b (int) __attribute__ ((noipa, alias ("a")));
static int w (int) __attribute__ ((weakref ("b")));

int
use_w (void)
{
  return w (2);
}

/* { dg-final { scan-ipa-dump "Optimizing weakref w\[^\n\]* as transparent alias" "visibility" } } */
/* { dg-final { scan-tree-dump "\[bw\] \\(2\\)" "optimized" } } */
