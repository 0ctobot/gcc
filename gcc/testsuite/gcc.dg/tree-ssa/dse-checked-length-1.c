/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-dse1" } */

typedef __SIZE_TYPE__ size_t;
struct words { unsigned char a[96], b[32]; };

void
clear (struct words *p, size_t size)
{
  __builtin___memset_chk (p, 0, sizeof *p, size);
  __builtin_memset (p->b, 1, sizeof p->b);
}

/* { dg-final { scan-tree-dump {__builtin___memset_chk \([^;]*, 0, 128, } "dse1" } } */
