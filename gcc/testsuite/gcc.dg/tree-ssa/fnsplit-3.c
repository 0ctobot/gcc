/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-fnsplit-details" } */

/* Each function below has a cheap early exit followed by a large tail,
   which ipa-split would normally move into a split part.  The tail reads
   the current frame, so it has to stay in the function's own frame.  */

extern void record (void *);
extern void work (int);

#define TAIL							\
  work (1); work (2); work (3); work (4); work (5); work (6);	\
  work (7); work (8); work (9); work (10); work (11); work (12);	\
  work (13); work (14); work (15); work (16)

static inline __attribute__ ((always_inline)) void
ignore (void *p)
{
}

void
ra (int *p)
{
  if (!p)
    return;
  record (__builtin_return_address (0));
  TAIL;
}

void
fa (int *p)
{
  if (!p)
    return;
  record (__builtin_frame_address (0));
  TAIL;
}

void
cfa (int *p)
{
  if (!p)
    return;
  record (__builtin_dwarf_cfa ());
  TAIL;
}

void
sa (int *p)
{
  if (!p)
    return;
  record (__builtin_stack_address ());
  TAIL;
}

void
unused (int *p)
{
  if (!p)
    return;
  ignore (__builtin_return_address (0));
  TAIL;
}

void
use (int *p)
{
  ra (p);
  ra (p + 1);
  fa (p);
  fa (p + 1);
  cfa (p);
  cfa (p + 1);
  sa (p);
  sa (p + 1);
  unused (p);
  unused (p + 1);
}

/* { dg-final { scan-tree-dump-times "Cannot split: builtin reading the frame" 4 "fnsplit" } } */
/* { dg-final { scan-tree-dump-times "Splitting function at:" 1 "fnsplit" } } */
