/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero --param sra-max-scalarization-size-Ospeed=64 -fdump-tree-optimized" } */

/* With -ftrivial-auto-var-init=zero, the padding of a local aggregate reads
   as zero.  Total scalarization mustn't rewrite a copy of a padded local as
   one store per field, which would leave the padding of the destination
   unwritten, so the copy into *pd has to stay.  A struct without padding is
   still scalarized and its copy into *nd goes away.  */

struct pad { char type; long cookie; short flags; };
struct nopad { long a; long b; long c; };

void
copy_pad (struct pad *pd, char type, long cookie, short flags)
{
  struct pad r;

  r.type = type;
  r.cookie = cookie;
  r.flags = flags;
  *pd = r;
}

void
copy_nopad (struct nopad *nd, long a, long b, long c)
{
  struct nopad n;

  n.a = a;
  n.b = b;
  n.c = c;
  *nd = n;
}

/* { dg-final { scan-tree-dump "\\*pd_\[0-9\]+\\(D\\) = r;" "optimized" } } */
/* { dg-final { scan-tree-dump-not "\\*nd_\[0-9\]+\\(D\\) = n;" "optimized" } } */
