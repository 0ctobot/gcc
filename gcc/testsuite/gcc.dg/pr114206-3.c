/* PR tree-optimization/114206 */
/* -Wmaybe-musttail-local-addr must only warn when an argument can really hold
   the address of a live local.  */
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
  __attribute__ ((musttail)) return g (n, p->id);
}

int
m_call (int n)
{
  int local = n;
  if (use (&local))
    return 0;
  int v = get ();
  __attribute__ ((musttail)) return g (n, v);
}

int
m_arith (int n)
{
  int local = n;
  if (use (&local))
    return 0;
  __attribute__ ((musttail)) return g (n * 3, n - 1);
}

long
m_wide (long n)
{
  int local = n;
  if (use (&local))
    return 0;
  __attribute__ ((musttail)) return wide (n - 1);
}

int
m_narrowed (int n, long v)
{
  int local = n;
  if (use (&local))
    return 0;
  __attribute__ ((musttail)) return g (n, (int) (v * 3));
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
