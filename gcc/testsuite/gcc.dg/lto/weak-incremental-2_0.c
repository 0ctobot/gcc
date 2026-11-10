/* { dg-lto-do link } */
/* { dg-lto-options { { -O2 -flto -fwhole-program -fdump-tree-optimized } } } */
/* { dg-extra-ld-options { -r -nostdlib -flinker-output=nolto-rel } } */

/* With -fwhole-program, an incremental link resolves every symbol that
   nothing outside the LTO code refers to as LDPR_PREVAILING_DEF_IRONLY.
   Weak definitions that stay externally visible because of the
   externally_visible or used attribute still stay weak in the output, so
   the LTO code mustn't fold their initializers or inline them.  */

__attribute__ ((weak, externally_visible)) const int ws = 1;
__attribute__ ((weak, used)) const int wu = 1;

__attribute__ ((weak, externally_visible)) int
wf (int v)
{
  return v + 1;
}

/* Read the variables through pointers that only IPA propagation can
   resolve, one function per variable.  */

static __attribute__ ((noinline)) int
load (const int *p)
{
  return *p;
}

static __attribute__ ((noinline)) int
loadu (const int *p)
{
  return *p;
}

__attribute__ ((externally_visible)) int
get (void)
{
  return load (&ws);
}

__attribute__ ((externally_visible)) int
getu (void)
{
  return loadu (&wu);
}

__attribute__ ((externally_visible)) int
call (int v)
{
  return wf (v);
}

/* IPA-SRA passes the value that load and loadu read as an argument, so a
   folded initializer shows up as load.isra (1) rather than as a return of
   the constant.  Look for the loads of the variables instead.  */

/* { dg-final { scan-ltrans-tree-dump " = ws;" "optimized" } } */
/* { dg-final { scan-ltrans-tree-dump " = wu;" "optimized" } } */
/* { dg-final { scan-ltrans-tree-dump "wf \\(v" "optimized" } } */
