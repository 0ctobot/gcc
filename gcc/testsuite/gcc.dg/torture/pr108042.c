/* PR middle-end/108042 */
/* A weakref without a target is equivalent to weak, so the address of an
   undefined symbol declared this way has to compare equal to null at run
   time instead of being folded to nonzero.  */
/* { dg-do run } */
/* { dg-require-weak "" } */
/* This test requires support for undefined weak symbols.  */
/* { dg-skip-if "" { "hppa*-*-hpux*" "*-*-aix*" "nvptx-*-*" "*-*-darwin*" } } */
/* { dg-skip-if PR119369 { amdgcn-*-* } } */

extern void file_func (void) __attribute__ ((weakref));
void implicit_extern_func (void) __attribute__ ((weakref));
extern int file_var __attribute__ ((weakref));

extern void merged_func (void);
extern void merged_func (void) __attribute__ ((weakref));

extern int merged_var;
extern int merged_var __attribute__ ((weakref));

extern void called_func (void) __attribute__ ((weakref));

int
main (void)
{
  extern void block_func (void) __attribute__ ((weakref));
  extern int block_var __attribute__ ((weakref));

  if (file_func)
    __builtin_abort ();
  if (implicit_extern_func)
    __builtin_abort ();
  if (&file_var)
    __builtin_abort ();
  if (merged_func)
    __builtin_abort ();
  if (&merged_var)
    __builtin_abort ();
  if (block_func)
    __builtin_abort ();
  if (&block_var)
    __builtin_abort ();

  if (called_func)
    called_func ();

  return 0;
}
