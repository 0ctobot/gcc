/* A variable-sized typeof at file scope must not start a statement list, which
   would make the parser accept the statement expressions that follow.  */
/* { dg-do compile } */
/* { dg-options "-std=gnu11 -w" } */

int g (void);

__typeof__ (char[g ()]);

int a = sizeof ({ int t = 1; t; }); /* { dg-error "braced-group within expression allowed only inside a function" } */

int b = sizeof ({ __label__ q; static int s[2]; int t = 1; goto q; q: t; }); /* { dg-error "braced-group within expression allowed only inside a function" } */

__typeof__ (char[g ()]) *c; /* { dg-error "variably modified" } */

int d = sizeof ({ int t = 1; t; }); /* { dg-error "braced-group within expression allowed only inside a function" } */
