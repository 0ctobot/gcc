/* { dg-do compile } */
/* { dg-additional-options "-std=gnu11" } */

extern int g, b;
extern int *sp2_alias;
extern void set_v (void);

int *weak_sp = &b;

void
weak_empty (void)
{
  set_v ();
}

void
weak_def_empty (void)
{
  set_v ();
}

int *
weak_ret_s (void)
{
  return &g;
}

int *
weak_def_ret_s (void)
{
  return &g;
}

int *
ext_ret (void)
{
  return &g;
}

int *
weak_noipa_def_ret_s (void)
{
  return &g;
}

int *
weak_noipa_ret_s (void)
{
  return &g;
}

void
weak_ignore_arg (int *p)
{
  *p = 2;
}

void
weak_def_ignore_arg (int *p)
{
  *p = 2;
}

void
weak_def_no_params (int *p)
{
  *p = 2;
}

void
set_sp2_alias (void)
{
  sp2_alias = &b;
}
