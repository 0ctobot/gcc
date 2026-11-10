/* { dg-do compile } */

struct s2 { int a, b; };
struct s3 { char a, b, c; };
struct s12 { int a, b, c; };

/* These replace the weak definitions in weak-const-expand-1.c.  */
const struct s2 w2 = { 70, 80 };
const struct s3 w3 = { 10, 20, 30 };
const struct s12 w12 = { 40, 50, 60 };
