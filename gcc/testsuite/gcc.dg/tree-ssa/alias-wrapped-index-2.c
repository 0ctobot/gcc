/* With -fwrapv-pointer, elements of a 24-byte type at variable indices can
   overlap partially once the distance between them wraps the address, but
   int and unsigned int indices are less than 1 << 33 apart, which keeps such
   elements less than 1 << 38 bytes apart.  The store to f->d[j].a then can't
   change f->d[i].b, and the load of f->d[i].b still gets the value stored
   to it.  */
/* { dg-do compile } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -fwrapv-pointer -fno-strict-aliasing -fdump-tree-fre1" } */

struct s3 { long a, b, c; };
struct r3 { long n; struct s3 d[]; };

long
narrow (struct r3 *f, int i, int j)
{
  f->d[i].b = 1;
  f->d[j].a = 2;
  return f->d[i].b;
}

long
mixed (struct r3 *f, unsigned int i, int j)
{
  f->d[i].b = 1;
  f->d[j].a = 2;
  return f->d[i].b;
}

/* { dg-final { scan-tree-dump-times "return 1;" 2 "fre1" } } */
