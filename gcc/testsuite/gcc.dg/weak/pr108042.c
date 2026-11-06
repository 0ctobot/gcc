/* PR middle-end/108042 */
/* A weakref without a target on a public symbol is equivalent to weak, whether
   the symbol is merely declared or also defined.  A symbol that cannot be weak
   keeps the existing diagnostics.  */
/* { dg-do compile } */
/* { dg-require-weak "" } */
/* { dg-options "-fcommon" } */
/* { dg-skip-if "" { *-*-mingw* } } */
/* NVPTX's definition of weak looks different to normal.  */
/* { dg-skip-if "" { nvptx-*-* } } */
/* { dg-skip-if PR119369 { amdgcn-*-* } } */

/* { dg-final { scan-weak "ext_func" } } */
extern void ext_func (void) __attribute__ ((weakref));

/* { dg-final { scan-weak "ext_var" } } */
extern int ext_var __attribute__ ((weakref));

/* { dg-final { scan-weak "tentative_var" } } */
int tentative_var __attribute__ ((weakref));

/* { dg-final { scan-weak "defined_var" } } */
int defined_var __attribute__ ((weakref)) = 1;

/* { dg-final { scan-weak "defined_func" } } */
void defined_func (void) __attribute__ ((weakref));
void defined_func (void) { }

/* { dg-final { scan-weak "merged_func" } } */
void merged_func (void) { }
void merged_func (void) __attribute__ ((weakref));

/* { dg-final { scan-weak "block_func" } } */
/* { dg-final { scan-weak "block_var" } } */

/* A block-scope weakref with a target is still ignored, since alias is too.  */
/* { dg-final { scan-not-weak "block_target_func" } } */
/* { dg-final { scan-not-weak "block_alias_target" } } */

/* Static symbols and inline functions cannot be weak.  */
static int static_var __attribute__ ((weakref)); /* { dg-warning "attribute should be accompanied" } */
/* { dg-final { scan-not-weak "static_var" } } */
static int static_init_var __attribute__ ((weakref)) = 1; /* { dg-warning "attribute ignored because variable is initialized" } */
/* { dg-final { scan-not-weak "static_init_var" } } */
inline void inline_func (void) __attribute__ ((weakref));
inline void inline_func (void) { } /* { dg-warning "attribute ignored because function is defined" } */
/* { dg-final { scan-not-weak "inline_func" } } */

void
use (void)
{
  extern void block_func (void) __attribute__ ((weakref));
  extern int block_var __attribute__ ((weakref));
  extern void block_target_func (void) __attribute__ ((weakref ("block_alias_target"))); /* { dg-warning "attribute ignored" } */
  static int block_static_var __attribute__ ((weakref)); /* { dg-warning "attribute ignored" } */
  int block_auto_var __attribute__ ((weakref)); /* { dg-warning "attribute ignored" } */

  ext_func ();
  block_func ();
  block_target_func ();
  inline_func ();
  ext_var = block_var + static_var + static_init_var + block_static_var;
  block_auto_var = 0;
  (void) block_auto_var;
}
