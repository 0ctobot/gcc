/* { dg-do compile } */
/* { dg-options "-O2 -Wall -Wextra -Wno-init-self -ftrivial-auto-var-init=zero" } */

/* A self-initialization turns off -Wuninitialized for the variable unless
   -Winit-self is enabled.  That has to hold for the automatic initialization
   ahead of the initializer too, once the variable only lives in its result
   after its address is folded away.  */

int
self_addr (void)
{
  int x = x;		/* { dg-bogus "uninitialized" } */
  int *p = &x;
  return *p;		/* { dg-bogus "uninitialized" } */
}

int
self_addr_cond (int c)
{
  int x = x;		/* { dg-bogus "uninitialized" } */
  int *p = &x;
  if (c)
    *p = 3;
  return x;		/* { dg-bogus "uninitialized" } */
}

int
self_deref (void)
{
  int x = *&x;		/* { dg-bogus "uninitialized" } */
  return x;		/* { dg-bogus "uninitialized" } */
}

int
self_pragma (void)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winit-self"
  int x = x;		/* { dg-bogus "uninitialized" } */
#pragma GCC diagnostic pop
  int *p = &x;
  return *p;		/* { dg-bogus "uninitialized" } */
}

/* A read that isn't a self-initialization still warns.  */

int
self_add (void)
{
  int x = x + 1;	/* { dg-warning "is used uninitialized" } */
  return x;
}
