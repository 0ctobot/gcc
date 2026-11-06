/* { dg-lto-do link } */
/* { dg-require-effective-target shared } */
/* { dg-require-effective-target fpic } */
/* { dg-extra-ld-options { -shared } } */
/* { dg-lto-options { { -O2 -flto -fPIC -flto-partition=max -fsemantic-interposition -fdump-tree-optimized } } } */

/* G can be interposed at dynamic link time, so its call in F must stay even
   though the definition seen here is empty and sits in another partition.  */

void
g (void)
{
}

int
f (void)
{
  g ();
  return 42;
}

/* { dg-final { scan-ltrans-tree-dump "g \\(\\);" "optimized" } } */
