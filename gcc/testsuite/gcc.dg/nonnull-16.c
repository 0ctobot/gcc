/* Control for nonnull-15.c: with -fdelete-null-pointer-checks, the null
   checks on the parameters the nonnull attribute names are still deleted.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdelete-null-pointer-checks -fdump-tree-optimized" } */

#include "nonnull-15.c"

/* { dg-final { scan-tree-dump-not "fail_head \\(\\)" "optimized" { target { ! keeps_null_pointer_checks } } } } */
/* { dg-final { scan-tree-dump-not "fail_cmp \\(\\)" "optimized" { target { ! keeps_null_pointer_checks } } } } */
/* { dg-final { scan-tree-dump-not "fail_deref \\(\\)" "optimized" { target { ! keeps_null_pointer_checks } } } } */
/* { dg-final { scan-tree-dump-not "end_loop \\(\\)" "optimized" { target { ! keeps_null_pointer_checks } } } } */
/* { dg-final { scan-tree-dump-not "fail_static \\(\\)" "optimized" { target { ! keeps_null_pointer_checks } } } } */
/* { dg-final { scan-tree-dump-not "fail_inline \\(\\)" "optimized" { target { ! keeps_null_pointer_checks } } } } */
