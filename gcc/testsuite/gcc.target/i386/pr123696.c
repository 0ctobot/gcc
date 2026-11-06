/* { dg-do compile { target lp64 } } */
/* { dg-options "-O2 -fsanitize=kernel-hwaddress" } */

/* Kernel function pointers have the tag bits set, so calling through one must
   not clear them.  The load of the pointer itself stays checked.  */

struct g
{
  int a;
  void (*call) (void);
};

void
f1 (struct g *g)
{
  g->call ();
}

int
f2 (struct g *g)
{
  g->call ();
  return 1;
}

/* { dg-final { scan-assembler-times "__hwasan_load8_noabort" 2 } } */
/* { dg-final { scan-assembler-not "9079256848778919937" } } */
