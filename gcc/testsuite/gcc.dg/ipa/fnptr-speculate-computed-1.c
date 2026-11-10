/* Calls through a function pointer field must not be speculated to the one
   function whose address is stored to it when the field is also written with
   a value that can be another function.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-ipa-devirt" } */

typedef int (*fn_t) (int);
volatile int sink;

#define DEF(N) \
  struct ops##N { fn_t f; }; \
  int fa##N (int x) { sink = x; return N; } \
  int fb##N (int x) { sink = x + 1; return N; } \
  int __attribute__ ((noinline)) \
  call##N (struct ops##N *o, int x) { return o->f (x); }

/* A parameter, stored through a pointer to the non-const record while the
   call goes through a pointer to the const one.  */
struct ops1 { fn_t f; };
int fa1 (int x) { sink = x; return 1; }
const struct ops1 tbl1 = { fa1 };
void set1 (struct ops1 *p, fn_t g) { p->f = g; }
int __attribute__ ((noinline))
call1 (const struct ops1 *o, int x) { return o->f (x); }

/* A pointer loaded from another structure.  */
DEF (2)
struct src { fn_t g; };
void set2a (struct ops2 *p) { p->f = fa2; }
void set2b (struct ops2 *p, const struct src *s) { p->f = s->g; }

/* The result of a conditional.  */
DEF (3)
void set3a (struct ops3 *p) { p->f = fa3; }
void set3b (struct ops3 *p, int c, fn_t g) { p->f = c ? fa3 : g; }

/* The result of a call.  */
DEF (4)
extern fn_t get4 (void);
void set4a (struct ops4 *p) { p->f = fa4; }
void set4b (struct ops4 *p) { p->f = get4 (); }

/* A volatile store, as the kernel's WRITE_ONCE does.  */
DEF (5)
void set5a (struct ops5 *p) { p->f = fa5; }
void set5b (struct ops5 *p, fn_t g) { *(volatile fn_t *) &p->f = g; }

/* Other functions in an array of records and in a nested record.  */
DEF (6)
struct outer6 { int k; struct ops6 o; };
void set6 (struct ops6 *p) { p->f = fa6; }
const struct ops6 arr6[2] = { { fb6 }, { fb6 } };
const struct outer6 nest6 = { 1, { fb6 } };

/* { dg-final { scan-ipa-dump-not "=> fa1/" "devirt" } } */
/* { dg-final { scan-ipa-dump-not "=> fa2/" "devirt" } } */
/* { dg-final { scan-ipa-dump-not "=> fa3/" "devirt" } } */
/* { dg-final { scan-ipa-dump-not "=> fa4/" "devirt" } } */
/* { dg-final { scan-ipa-dump-not "=> fa5/" "devirt" } } */
/* { dg-final { scan-ipa-dump-not "=> fa6/" "devirt" } } */
/* { dg-final { scan-ipa-dump-not "-> speculative call" "devirt" } } */
