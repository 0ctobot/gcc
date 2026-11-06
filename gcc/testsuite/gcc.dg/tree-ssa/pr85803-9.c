/* A fault can only let a signal handler see a local variable once the address
   of that variable has been exposed. Before that point the fault doesn't keep
   a store to it alive, which lets the part of the initialization that's
   overwritten later on get trimmed.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-dse1-details" } */

struct ma { long a, b, c, d, e, f, g, h; };
void walk (struct ma *);

/* Exposed only after the faults: the dead head of m = {} goes away.  */
long
after_fault (long *p)
{
  struct ma m = { .a = p[1], .b = p[2] };
  walk (&m);
  return m.c;
}

/* A memset doesn't expose the address: same as above.  */
long
memset_no_exposure (long *p)
{
  struct ma m;
  __builtin_memset (&m, 0, sizeof (m));
  m.a = p[1];
  m.b = p[2];
  walk (&m);
  return m.c;
}

/* Exposed earlier in the same block: keep the whole memset.  */
long
before_fault (long *p)
{
  struct ma m;
  walk (&m);
  __builtin_memset (&m, 0, sizeof (m));
  m.a = p[1];
  m.b = p[2];
  walk (&m);
  return m.c;
}

/* Exposed in an earlier block on only one path to the faults.  */
long
before_fault_cond (long *p, int c)
{
  struct ma m;
  if (c)
    walk (&m);
  __builtin_memset (&m, 0, sizeof (m));
  m.a = p[1];
  m.b = p[2];
  walk (&m);
  return m.c;
}

/* The exposure later in the loop body precedes the faults through the back
   edge.  */
long
in_loop (long *p, int n)
{
  long s = 0;
  for (int i = 0; i < n; i++)
    {
      struct ma m = { .a = p[1], .b = p[2] };
      walk (&m);
      s += m.c;
    }
  return s;
}

/* { dg-final { scan-tree-dump-times "Trimming statement" 2 "dse1" } } */
