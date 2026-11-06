/* PR tree-optimization/85803 */
/* { dg-do run } */
/* { dg-additional-options "-fexceptions" } */
/* { dg-require-effective-target exceptions } */

/* An exception leaving the function has to see every store made in front of
   it to memory that the catcher can read, even when the statement throwing it
   or the GIMPLE_RESX continuing it has no use of that memory.  */

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <unwind.h>

static _Unwind_Reason_Code
stopfn (int version, _Unwind_Action actions, _Unwind_Exception_Class exc_class,
	struct _Unwind_Exception *exc_obj, struct _Unwind_Context *context,
	void *stop_parameter)
{
  if (actions & _UA_END_OF_STACK)
    abort ();
  return _URC_NO_REASON;
}

__attribute__ ((noinline, noclone)) int
cthrow (int i)
{
  if (i)
    {
      struct _Unwind_Exception *exc = malloc (sizeof (*exc));
      memset (&exc->exception_class, 0, sizeof (exc->exception_class));
      exc->exception_cleanup = 0;
#ifndef __USING_SJLJ_EXCEPTIONS__
      _Unwind_ForcedUnwind (exc, stopfn, 0);
#else
      _Unwind_SjLj_ForcedUnwind (exc, stopfn, 0);
#endif
      abort ();
    }
  return i + 1;
}

/* A call through this type has no virtual operands.  The compiler would just
   drop the call if it could see that a const function has a known result, which
   the volatile pointer hides from it.  */
typedef int (*cfn_t) (int) __attribute__ ((const));
static cfn_t volatile get_cthrow = (cfn_t) cthrow;

int x1, x2, x3, x4;
static int x5;
int y, sink;

int
read_x5 (void)
{
  return x5;
}

static inline void
nop_cleanup (char *p)
{
  __asm__ volatile ("");
}

static inline void
store_cleanup (char *p)
{
  y = 1;
}

static inline void
load_cleanup (int *p)
{
  *p = y;
  __asm__ volatile ("" : : "r" (*p));
}

/* The call has no virtual operands.  */
__attribute__ ((noinline, noclone)) int
f1 (cfn_t fn, int i)
{
  x1 = 42;
  int q = fn (i);
  x1 = 0;
  return q;
}

/* The path to the GIMPLE_RESX has no virtual operands.  */
__attribute__ ((noinline, noclone)) int
f2 (cfn_t fn, int i)
{
  x2 = 42;
  int q;
  {
    char d __attribute__ ((cleanup (nop_cleanup)));
    q = fn (i);
  }
  x2 = 0;
  return q;
}

/* The path to the GIMPLE_RESX stores something else.  */
__attribute__ ((noinline, noclone)) int
f3 (cfn_t fn, int i)
{
  x3 = 42;
  int q;
  {
    char d __attribute__ ((cleanup (store_cleanup)));
    q = fn (i);
  }
  x3 = 0;
  return q;
}

/* The path to the GIMPLE_RESX loads something else.  */
__attribute__ ((noinline, noclone)) int
f4 (cfn_t fn, int i)
{
  x4 = 42;
  int q;
  {
    int d __attribute__ ((cleanup (load_cleanup)));
    q = fn (i);
  }
  x4 = 0;
  return q;
}

__attribute__ ((noinline, noclone)) void
thrower (cfn_t fn, int i)
{
  sink = fn (i);
}

/* The callee has a virtual use, but it doesn't read x5.  */
__attribute__ ((noinline, noclone)) void
f5 (cfn_t fn, int i)
{
  x5 = 42;
  thrower (fn, i);
  x5 = 0;
}

static jmp_buf jb;
static int *volatile watch;

static void
check (char *p)
{
  if (*(volatile int *) watch != 42)
    abort ();
  longjmp (jb, 1);
}

#define RUN(CALL, VAR)						\
  do								\
    {								\
      watch = &VAR;						\
      if (!setjmp (jb))						\
	{							\
	  char d __attribute__ ((cleanup (check)));		\
	  CALL;							\
	  abort ();						\
	}							\
    }								\
  while (0)

__attribute__ ((noinline, noclone)) void
run1 (void)
{
  RUN (f1 (get_cthrow, 1), x1);
}

__attribute__ ((noinline, noclone)) void
run2 (void)
{
  RUN (f2 (get_cthrow, 1), x2);
}

__attribute__ ((noinline, noclone)) void
run3 (void)
{
  RUN (f3 (get_cthrow, 1), x3);
}

__attribute__ ((noinline, noclone)) void
run4 (void)
{
  RUN (f4 (get_cthrow, 1), x4);
}

__attribute__ ((noinline, noclone)) void
run5 (void)
{
  RUN (f5 (get_cthrow, 1), x5);
}

int
main (void)
{
  run1 ();
  run2 ();
  run3 ();
  run4 ();
  run5 ();
  return sink;
}
