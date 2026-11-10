/* The zinit pass only deletes a zero store if it's the first statement to write
   memory after the variable's .DEFERRED_INIT call.  Here the call to get ()
   writes memory first, so the memset of t.y stays although .DEFERRED_INIT
   already zeroed those bytes.  */
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
