/* { dg-do compile } */
/* { dg-options "-O2" } */

struct S { int n; int a[]; };

/* These replace the weak definitions in builtin-object-size-weak-1.c.  */
struct S w = { 4, { 1, 2, 3, 4 } };
struct S v = { 1, { 1 } };
