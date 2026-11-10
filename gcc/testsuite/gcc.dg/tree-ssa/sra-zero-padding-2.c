/* { dg-do compile } */
/* { dg-options "-O2 -fzero-init-padding-bits=all --param sra-max-scalarization-size-Ospeed=64 -fdump-tree-optimized" } */

/* Same as sra-zero-padding-1.c with -fzero-init-padding-bits=all, which
   zeroes the padding of a local with an initializer: the copy of the padded
   local into *pd has to stay, and the struct without padding is still
   scalarized.  */

struct pad { char type; long cookie; short flags; };
struct nopad { long a; long b; long c; };

void
copy_pad (struct pad *pd, char type, long cookie, short flags)
{
  struct pad r = {};

  r.type = type;
  r.cookie = cookie;
  r.flags = flags;
  *pd = r;
}

void
copy_nopad (struct nopad *nd, long a, long b, long c)
{
  struct nopad n = {};

  n.a = a;
  n.b = b;
  n.c = c;
  *nd = n;
}

/* { dg-final { scan-tree-dump "\\*pd_\[0-9\]+\\(D\\) = r;" "optimized" } } */
/* { dg-final { scan-tree-dump-not "\\*nd_\[0-9\]+\\(D\\) = n;" "optimized" } } */
