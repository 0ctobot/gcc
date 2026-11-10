/* PR middle-end/128302 */
/* A union initialized through a member that spans the whole union needs no
   clear ahead of the member store, while a bit-field narrower than the union
   does.  */
/* { dg-do compile } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-O2 -fzero-init-padding-bits=all -fdump-tree-gimple" } */

union full_bf { int f : 32; };
union full_int { int i; short s; };
union full_struct { struct { unsigned int a : 8, b : 24; } s; unsigned int raw; };
union full_long { long long l; int f : 24; };
union narrow { int f : 24; };

void sink (void *);

void
f1 (int v)
{
  union full_bf d1 = { .f = v };
  sink (&d1);
}

void
f2 (int v)
{
  union full_int d2 = { .i = v };
  sink (&d2);
}

void
f3 (int v)
{
  union full_struct d3 = { .s = { .a = v, .b = v } };
  sink (&d3);
}

void
f4 (long long v)
{
  union full_long d4 = { .l = v };
  sink (&d4);
}

void
f5 (int v)
{
  union narrow n5 = { .f = v };
  sink (&n5);
}

/* { dg-final { scan-tree-dump-not "d\[0-9\] = {}" "gimple" } } */
/* { dg-final { scan-tree-dump-times "n5 = {}" 1 "gimple" } } */
