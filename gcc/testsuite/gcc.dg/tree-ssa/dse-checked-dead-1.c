/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-dse1" } */

typedef __SIZE_TYPE__ size_t;
struct words { unsigned char bytes[128]; };

void
all_written (struct words *p, size_t size)
{
  __builtin___memset_chk (p, 0, sizeof *p, size);
  __builtin_memset (p, 1, sizeof *p);
}

void
already_zero (struct words *p, size_t size)
{
  __builtin_memset (p, 0, sizeof *p);
  __builtin___memset_chk (p, 0, sizeof *p, size);
}

/* The second clear only runs once the check has passed, so it's still
   redundant.  */

void
later_clear (struct words *p, size_t size)
{
  __builtin___memset_chk (p, 0, sizeof *p, size);
  __builtin_memset (p, 0, 64);
}

/* { dg-final { scan-tree-dump-times {__builtin___memset_chk \([^;]*, 0, 128, } 3 "dse1" } } */
/* { dg-final { scan-tree-dump-not {__builtin_memset \([^;]*, 0, 64\)} "dse1" } } */
