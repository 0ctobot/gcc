/* PR tree-optimization/127263 */
/* { dg-do compile } */
/* { dg-options "-O2 -fchecking" } */

/* Predictive commoning replaced a load with the value of a store to the same
   location even though the two access it through types that only convert
   uselessly in one direction.  */

typedef _Bool (*fp_t) ();

_Bool ob;

void
load_fp_store_void (void)
{
  fp_t *p;
lbl:
  ob = *p;
  *(void **) p = 0;
  if (ob)
    goto lbl;
}

void
load_void_store_fp (void)
{
  void **p;
lbl:
  ob = *p;
  *(fp_t *) p = 0;
  if (ob)
    goto lbl;
}
