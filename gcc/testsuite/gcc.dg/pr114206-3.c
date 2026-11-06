/* PR tree-optimization/114206 */
/* -Wmaybe-musttail-local-addr must only warn when an argument can really hold
   the address of a live local.  Getting past a branch on what USE returned
   tells which way it went, which can confirm a guess of the address, so after
   such a branch any argument that isn't constant can hold it.  */
/* { dg-do compile { target musttail } } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -Wextra" } */

__attribute__ ((pure)) int use (int *);
extern int g (int, int);
extern int get (void);
extern long wide (long);
struct d { int id; };

int
m_load (struct d *p, int n)
{
  int local = n;
  if (use (&local))
    return 0;
  __attribute__ ((musttail)) return g (n, p->id); /* { dg-warning "musttail" } */
}

int
m_call (int n)
{
  int local = n;
  if (use (&local))
    return 0;
  int v = get ();
  __attribute__ ((musttail)) return g (n, v); /* { dg-warning "musttail" } */
}

int
m_arith (int n)
{
  int local = n;
  if (use (&local))
    return 0;
  __attribute__ ((musttail)) return g (n * 3, n - 1); /* { dg-warning "musttail" } */
}

long
m_wide (long n)
{
  int local = n;
  if (use (&local))
    return 0;
  __attribute__ ((musttail)) return wide (n - 1); /* { dg-warning "musttail" } */
}

int
m_narrowed (int n, long v)
{
  int local = n;
  if (use (&local))
    return 0;
  __attribute__ ((musttail)) return g (n, (int) (v * 3)); /* { dg-warning "musttail" } */
}

int
m_const (int n)
{
  int local = n;
  if (use (&local))
    return 0;
  __attribute__ ((musttail)) return g (1, 2);
}

int
m_piece (int n)
{
  int local = n;
  unsigned long a = (unsigned long) &local;
  if (use (&local))
    return 0;
  __attribute__ ((musttail)) return g ((int) a, (int) (a >> 32)); /* { dg-warning "musttail" } */
}
