/* { dg-do compile } */
/* { dg-options "-ftrivial-auto-var-init=zero -fdump-tree-gimple" } */

/* The comma operator sequences a full store before the following reads.  The
   store doesn't cover them if it might not run, if it only assigns one field
   or if it reads the variable to compute the value it writes.  */

int
store_first (void)
{
  volatile int seq = (seq = 1, seq);
  return seq;
}

int
comma_chain (int a)
{
  volatile int chain = (chain = a + 1, chain++, chain + 2);
  return chain;
}

int
read_rhs (void)
{
  int rhs = (rhs = rhs + 1, rhs);
  return rhs;
}

int
short_circuit (int c)
{
  volatile int sc = (c && (sc = 1), sc);
  return sc;
}

int
conditional (int c)
{
  volatile int cond = (c ? (cond = 1) : 0, cond);
  return cond;
}

int
jump_out (int c)
{
  volatile int jump = (jump = ({ if (c) goto out; 1; }), jump);
out:
  return jump;
}

struct S { int a, b; };

int
partial_store (void)
{
  volatile struct S part = (part.a = 1, (struct S) { 2, part.b });
  return part.b;
}

/* { dg-final { scan-tree-dump-not "\"seq\"" "gimple" } } */
/* { dg-final { scan-tree-dump-not "\"chain\"" "gimple" } } */
/* { dg-final { scan-tree-dump-times "\"rhs\"" 1 "gimple" } } */
/* { dg-final { scan-tree-dump-times "\"sc\"" 1 "gimple" } } */
/* { dg-final { scan-tree-dump-times "\"cond\"" 1 "gimple" } } */
/* { dg-final { scan-tree-dump-times "\"jump\"" 1 "gimple" } } */
/* { dg-final { scan-tree-dump-times "\"part\"" 1 "gimple" } } */
