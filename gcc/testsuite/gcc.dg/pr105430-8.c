/* PR c/105430 */
/* Designators in reverse order over a long run of members that share a
   position.  Comparing two of them can't take time that depends on how many
   members lie between them, or this takes minutes rather than a moment.  */
/* { dg-do compile } */
/* { dg-options "-O0 -w -fsyntax-only" } */
/* { dg-timeout 60 } */

struct E { };
struct E fe (void);
void use (void *);

#define L1(x, p) x (p##0) x (p##1) x (p##2) x (p##3)
#define L2(x, p) L1 (x, p##0) L1 (x, p##1) L1 (x, p##2) L1 (x, p##3)
#define L3(x, p) L2 (x, p##0) L2 (x, p##1) L2 (x, p##2) L2 (x, p##3)
#define L4(x, p) L3 (x, p##0) L3 (x, p##1) L3 (x, p##2) L3 (x, p##3)
#define L5(x, p) L4 (x, p##0) L4 (x, p##1) L4 (x, p##2) L4 (x, p##3)
#define L6(x, p) L5 (x, p##0) L5 (x, p##1) L5 (x, p##2) L5 (x, p##3)
#define L7(x, p) L6 (x, p##0) L6 (x, p##1) L6 (x, p##2) L6 (x, p##3)
#define L8(x, p) L7 (x, p##0) L7 (x, p##1) L7 (x, p##2) L7 (x, p##3)

#define R1(x, p) x (p##3) x (p##2) x (p##1) x (p##0)
#define R2(x, p) R1 (x, p##3) R1 (x, p##2) R1 (x, p##1) R1 (x, p##0)
#define R3(x, p) R2 (x, p##3) R2 (x, p##2) R2 (x, p##1) R2 (x, p##0)
#define R4(x, p) R3 (x, p##3) R3 (x, p##2) R3 (x, p##1) R3 (x, p##0)
#define R5(x, p) R4 (x, p##3) R4 (x, p##2) R4 (x, p##1) R4 (x, p##0)
#define R6(x, p) R5 (x, p##3) R5 (x, p##2) R5 (x, p##1) R5 (x, p##0)
#define R7(x, p) R6 (x, p##3) R6 (x, p##2) R6 (x, p##1) R6 (x, p##0)
#define R8(x, p) R7 (x, p##3) R7 (x, p##2) R7 (x, p##1) R7 (x, p##0)

#define MEMBER(n) struct E e##n;
#define INIT(n) .e##n = fe (),

struct A { L8 (MEMBER, m) };

void
f (void)
{
  struct A a = { R8 (INIT, m) };
  use (&a);
}
