/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto -fipa-pta -fdump-tree-optimized } } } */

/* pub can be called back from ext, so the calls to ext clobber what pub
   clobbers.  pub calls through a static table of functions that clobber
   nothing, so val, whose address only reaches parse, stays in a register
   across the calls to ext and is loaded once after parse.  parse is in
   another unit, so only the link-time points-to analysis knows that val
   doesn't escape.  If the table's initializer isn't read in the LTRANS
   unit, the table can point to anything, so pub and the calls to ext
   clobber val.  */

extern int ext (int);
extern int parse (const char *, unsigned char *);

__attribute__ ((noinline)) int
store (const char *buf)
{
  unsigned char val;

  if (parse (buf, &val))
    return -1;
  ext (val);
  ext (val);
  return val;
}

__attribute__ ((noinline)) static void
n1 (void)
{
}

__attribute__ ((noinline)) static void
n2 (void)
{
}

static void (*const tbl[2]) (void) = { n1, n2 };
extern int sel;

void
pub (void)
{
  tbl[sel & 1] ();
}

int
main (int argc, char **argv)
{
  if (store (argv[0]) != (unsigned char) argv[0][0])
    __builtin_abort ();
  return 0;
}

/* { dg-final { scan-ltrans-tree-dump-times " = val;" 1 "optimized" } } */
