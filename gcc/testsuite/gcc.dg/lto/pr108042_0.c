/* PR middle-end/108042 */
/* A weakref without a target is equivalent to weak, so it has to resolve to a
   definition from another translation unit and to null if there isn't one.  */
/* { dg-lto-do run } */
/* { dg-require-weak "" } */
/* This test requires support for undefined weak symbols.  */
/* { dg-skip-if "" { "hppa*-*-hpux*" "*-*-aix*" "nvptx-*-*" "*-*-darwin*" } } */
/* { dg-skip-if PR119369 { amdgcn-*-* } } */

extern void undefined_func (void) __attribute__ ((weakref));
extern int undefined_var __attribute__ ((weakref));
extern void other_tu_func (void) __attribute__ ((weakref));
extern int other_tu_var __attribute__ ((weakref));

int
main (void)
{
  if (undefined_func || &undefined_var)
    __builtin_abort ();
  if (!other_tu_func || !&other_tu_var)
    __builtin_abort ();
  other_tu_func ();
  if (other_tu_var != 2)
    __builtin_abort ();
  return 0;
}
