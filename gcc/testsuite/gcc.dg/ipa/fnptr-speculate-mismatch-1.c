/* PR target/124318 */
/* A function stored in a record through a cast doesn't have the signature of a
   call through that field, so it mustn't become a speculative direct call that
   the inliner and IPA-CP then follow.  The control's call is still
   speculated.  */
/* { dg-do compile } */
/* { dg-options "-O2 -std=gnu17 -fdump-ipa-devirt-details" } */

struct big { long a[8]; };

struct ops
{
  int (*control) (int);
  int (*fewer) (long, long);
  void (*more) (int);
  int (*big_ret) (void);
  int (*vararg) (int, ...);
  long (*big_arg) (long);
  int (*no_proto) ();
  int (*in_code) (int);
  int (*call_cast) (int);
};

extern volatile int sink;

int t_control (int a) { return a + sink; }
int t_fewer (int *p) { return *p + sink; }
int t_more (int a, int b, int c) { sink = a + b + c; return c; }
struct big t_big (void) { struct big b = { { sink } }; return b; }
int t_novar (int a, int b) { return a - b + sink; }
long t_byval (struct big b) { return b.a[3] + sink; }
int t_kr (a, b) int a, b; { return a + b + sink; }
int t_cast (long a) { return a + sink; }
int t_good (int a) { return a + sink; }
int t_call_cast (int a) { return a + sink; }

struct ops table = {
  t_control,
  (int (*) (long, long)) t_fewer,
  (void (*) (int)) t_more,
  (int (*) (void)) t_big,
  (int (*) (int, ...)) t_novar,
  (long (*) (long)) t_byval,
  (int (*) ()) t_kr,
  0,
  t_call_cast,
};

struct ops other;

void set_good (void) { other.in_code = t_good; }
void set_cast (void) { other.in_code = (int (*) (int)) t_cast; }

int c_control (struct ops *p) { return p->control (1); }
int c_fewer (struct ops *p) { return p->fewer (1, 2); }
void c_more (struct ops *p) { p->more (1); }
int c_big_ret (struct ops *p) { return p->big_ret (); }
int c_vararg (struct ops *p) { return p->vararg (1, 2.0, 7); }
long c_big_arg (struct ops *p) { return p->big_arg (1); }
int c_no_proto (struct ops *p) { return p->no_proto (1); }
int c_in_code (struct ops *p) { return p->in_code (1); }
int c_call_cast (struct ops *p) { return ((int (*) (long, long)) p->call_cast) (1, 2); }

/* { dg-final { scan-ipa-dump-times "speculatively turning an indirect call" 1 "devirt" } } */
/* { dg-final { scan-ipa-dump "direct one to t_control" "devirt" } } */
