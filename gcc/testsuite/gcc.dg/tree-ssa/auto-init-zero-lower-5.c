/* The zero-init lowering only deletes a zero store that is the first
   statement after the marker to write memory.  Here the call writes
   memory first, so the memset of t.y stays although the marker already
   zeroed those bytes.  */
/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero -fdump-tree-zinit1-details" } */

struct two { int x; long y[6]; };
extern void sink (void *);
extern int get (void);

void
f (void)
{
  struct two t;
  int v = get ();
  __builtin_memset (&t.y, 0, sizeof t.y);
  t.x = v;
  sink (&t);
}

/* { dg-final { scan-tree-dump "Deleted redundant call: __builtin_memset" "zinit1" { xfail *-*-* } } } */
