/* { dg-do compile { target lp64 } } */
/* { dg-options "-O2 -fsanitize=hwaddress" } */

/* Userspace hwasan still clears the tag from an indirect call's address.  */

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

/* { dg-final { scan-assembler "9079256848778919937" } } */
