/* A compound literal with static storage is handled as if it were the
   brace-enclosed list it contains.  As with that list, its data for a flexible
   array member can't initialize a subobject, while a literal without such data
   can.  */
/* { dg-do compile } */
/* { dg-options "-std=gnu11" } */

struct F { int a; int f[]; };
struct S { int a; char s[]; };
struct WF { struct F f; int z; };
struct WL { struct F f; };
struct WS { struct S s; int z; };
struct WW { struct WL l; int z; };
union U { struct F f; int x[3]; };

struct WF e0 = { .f = (struct F) { 1, { 2, 3 } } }; /* { dg-error "flexible array member in a nested context" } */
struct WF e1 = { .f = (struct F) { 1, { 2, 3 } }, .f.a = 9 }; /* { dg-error "flexible array member in a nested context" } */
struct WF e2 = { .f = (struct F) { 1, { 2, 3 } }, .z = 3 }; /* { dg-error "flexible array member in a nested context" } */
struct WL e3 = { .f = (struct F) { 1, { 2, 3 } } }; /* { dg-error "flexible array member in a nested context" } */
struct WS e4 = { .s = (struct S) { 1, "abc" } }; /* { dg-error "flexible array member in a nested context" } */
struct F e5[1] = { (struct F) { 1, { 2 } } }; /* { dg-error "flexible array member in a nested context" } */
union U e6 = { .f = (const struct F) { 1, { 2 } } }; /* { dg-error "flexible array member in a nested context" } */

struct WF g0 = { .f = (struct F) { 1 } };
struct WF g1 = { .f = (struct F) { 1, {} }, .z = 3 };
struct WW g2 = { .l = (struct WL) { .f = (struct F) { 1 } }, .z = 3 };
