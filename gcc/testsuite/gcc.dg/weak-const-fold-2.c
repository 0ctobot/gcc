/* The value of a weak const object isn't known until link time, so it
   can't be used in an initializer for an object with static storage
   duration.  */
/* { dg-do compile } */
/* { dg-require-weak "" } */
/* { dg-options "" } */

struct S { int a, b; };

__attribute__ ((weak)) const int x = 5;
__attribute__ ((weak)) const struct S ws = { 1, 2 };
const int y = 6;

int ix = x;		/* { dg-error "initializer element is not constant" } */
struct S is = ws;	/* { dg-error "initializer element is not constant" } */
int iy = y;
