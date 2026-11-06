/* PR c/105430 */
/* { dg-do compile } */
/* { dg-require-effective-target size32plus } */
/* { dg-options "-std=gnu23" } */
/* { dg-timeout 30 } */

/* Mapping the fields of a same-tag compound literal onto the type it initializes
   must not take time quadratic in the number of fields.  This takes over 100
   seconds when it does, and a few at most when it doesn't.  */

#define F1(p) int p##0, p##1, p##2, p##3, p##4, p##5, p##6, p##7;
#define F2(p) F1(p##0) F1(p##1) F1(p##2) F1(p##3) F1(p##4) F1(p##5) F1(p##6) F1(p##7)
#define F3(p) F2(p##0) F2(p##1) F2(p##2) F2(p##3) F2(p##4) F2(p##5) F2(p##6) F2(p##7)
#define F4(p) F3(p##0) F3(p##1) F3(p##2) F3(p##3) F3(p##4) F3(p##5) F3(p##6) F3(p##7)
#define F5(p) F4(p##0) F4(p##1) F4(p##2) F4(p##3) F4(p##4) F4(p##5) F4(p##6) F4(p##7)
#define FIELDS F5(f0) F5(f1) F5(f2) F5(f3)

#define V1 1, 2, 3, 4, 5, 6, 7, 8
#define V2 V1, V1, V1, V1, V1, V1, V1, V1
#define V3 V2, V2, V2, V2, V2, V2, V2, V2
#define V4 V3, V3, V3, V3, V3, V3, V3, V3
#define V5 V4, V4, V4, V4, V4, V4, V4, V4
#define VALUES V5, V5, V5, V5

typedef struct T { FIELDS } OT;

int
f (void)
{
  struct T { FIELDS };
  static struct W { struct T s; } a = { .s = (OT) { VALUES }, .s.f100000 = 9 };
  return a.s.f100000;
}
