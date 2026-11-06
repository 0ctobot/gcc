/* PR tree-optimization/126197 */
/* { dg-do compile } */

/* The noreturn fixup of the indirect call splits the block that holds the
   condition with the __builtin_unreachable arm, so remembering that condition
   by its block numbers no longer finds its edge.  */

typedef __INT32_TYPE__ int32_t;
typedef void *(*fp_t) ();
int32_t g19;
void *f29 ();

void *
f7 ()
{
  __builtin_abort ();
}

void
f12 ()
{
  int32_t bb0;
  int32_t bb1;
  _Bool c7, c11;
  fp_t fp10;
  bb0 = __builtin_clz (g19);
  switch ((int32_t) bb0)
  case 5015:
  case 309795350:
    goto lbl_bf45;
  fp10 = f7;
lbl_br5:
  if (c7)
  lbl_bf16:
    if (bb1)
      goto lbl_br5;
  fp10 ();
  if (c7)
    __builtin_unreachable ();
  fp10 = f29;
  if (c11)
    return;
lbl_bf45:
  c11 = 1;
  goto lbl_bf16;
}
