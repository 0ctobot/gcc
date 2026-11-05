/* PR rtl-optimization/128427 */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-code-hoisting -fno-tree-pre -fno-caller-saves" } */

/* Head merging can still move a division above const and pure calls that
   always return, and it can move an insn that doesn't trap above anything.  */

/* -fno-caller-saves keeps the operands and the result in call-saved registers.
   Otherwise the two copies can use different registers depending on the tuning,
   and they wouldn't be candidates for head merging in the first place.  */

extern volatile int sink, vload;
extern void g (void);
extern void maybe_exit (void);
extern int cfn (int) __attribute__ ((const));
extern int pfn (int) __attribute__ ((pure));

int
across_const (int c, int x, int y, int n)
{
  int r;

  if (c)
    goto out;
  if (cfn (n))
    {
      r = x / y;
      g ();
      sink += r + x + y;
      return r - 1;
    }
out:
  r = x / y;
  g ();
  return r * 3 + x + y;
}

int
across_pure (int c, int x, int y, int n)
{
  int r;

  if (c)
    goto out;
  if (pfn (n))
    {
      r = x / y;
      g ();
      sink += r + x + y;
      return r - 1;
    }
out:
  r = x / y;
  g ();
  return r * 3 + x + y;
}

int
no_trap_across_call (int c, int x, int y)
{
  int r;

  if (c)
    goto out;
  maybe_exit ();
  if (vload)
    {
      r = x ^ y;
      g ();
      sink += r + x + y;
      return r - 1;
    }
out:
  r = x ^ y;
  g ();
  return r * 3 + x + y;
}

/* { dg-final { scan-assembler-times {\tsdiv\t} 2 } } */
/* { dg-final { scan-assembler-times {\teor\t} 1 } } */
