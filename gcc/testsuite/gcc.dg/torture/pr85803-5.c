/* PR tree-optimization/85803 */
/* A store through a pointer must stay visible when a later store through the
   same pointer faults at an offset outside the object that the pointer points
   to.  */
/* { dg-do run { target *-*-linux* } } */
/* { dg-additional-options "-fno-schedule-insns -fno-schedule-insns2 -fno-schedule-fusion" } */

#include <setjmp.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>

static sigjmp_buf env;

static void
handler (int sig)
{
  siglongjmp (env, 1);
}

__attribute__ ((noinline, noclone)) void
near_first (long *p)
{
  p[0] = 42;
  p[512] = 7;
  p[0] = 1;
}

__attribute__ ((noinline, noclone)) void
adjacent (long *p)
{
  p[0] = 42;
  p[1] = 7;
  p[0] = 1;
}

__attribute__ ((noinline, noclone)) void
far_first (long *p)
{
  p[512] = 42;
  p[0] = 7;
  p[512] = 1;
}

__attribute__ ((noinline, noclone)) void
variable_index (long *p, long i)
{
  p[0] = 42;
  p[i] = 7;
  p[0] = 1;
}

__attribute__ ((noinline, noclone)) void
small_index (char *p)
{
  p[0] = 42;
  p[100] = 7;
  p[0] = 1;
}

int
main (void)
{
  long page = sysconf (_SC_PAGESIZE);
  char *m = mmap (0, 3 * page, PROT_READ | PROT_WRITE,
		  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  long *p, n = page / sizeof (long);

  if (m == MAP_FAILED || page != 4096 || mprotect (m + page, page, PROT_NONE))
    return 0;

  signal (SIGSEGV, handler);

  p = (long *) m;
  if (!sigsetjmp (env, 1))
    {
      near_first (p);
      __builtin_abort ();
    }
  if (p[0] != 42)
    __builtin_abort ();

  p = (long *) (m + page - sizeof (long));
  p[0] = 0;
  if (!sigsetjmp (env, 1))
    {
      adjacent (p);
      __builtin_abort ();
    }
  if (p[0] != 42)
    __builtin_abort ();

  /* The far store lands on the last page here, and the near store faults.  */
  p = (long *) (m + page);
  if (!sigsetjmp (env, 1))
    {
      far_first (p);
      __builtin_abort ();
    }
  if (p[512] != 42)
    __builtin_abort ();

  p = (long *) m;
  p[0] = 0;
  if (!sigsetjmp (env, 1))
    {
      variable_index (p, n);
      __builtin_abort ();
    }
  if (p[0] != 42)
    __builtin_abort ();

  if (!sigsetjmp (env, 1))
    {
      small_index (m + page - 100);
      __builtin_abort ();
    }
  if (m[page - 100] != 42)
    __builtin_abort ();

  return 0;
}
