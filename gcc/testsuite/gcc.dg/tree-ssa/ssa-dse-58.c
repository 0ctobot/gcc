/* The memset in t_memset and the zero initialization of buf in t_init are dead
   since the copy into buf overwrites all of it before anything reads it.  The
   walk over the uses of each store forks at every if/else and only reaches the
   copy after the last join.  Following each path separately would walk what
   comes after a join once for every path reaching it, which would use up the
   alias query limit after a handful of forks.  */
/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero -fdump-tree-dse1-details" } */

extern void f0 (void);
extern void f1 (void);

#define FORKS					\
  if (c0) f0 (); else f1 ();			\
  if (c1) f0 (); else f1 ();			\
  if (c2) f0 (); else f1 ();			\
  if (c3) f0 (); else f1 ();			\
  if (c4) f0 (); else f1 ();			\
  if (c5) f0 (); else f1 ();			\
  if (c6) f0 (); else f1 ();			\
  if (c7) f0 (); else f1 ();

int
t_memset (const int *src, int i, int c0, int c1, int c2, int c3, int c4,
	  int c5, int c6, int c7)
{
  int buf[8];
  __builtin_memset (buf, 0, sizeof buf);
  FORKS
  __builtin_memcpy (buf, src, sizeof buf);
  return buf[i & 7];
}

int
t_init (const int *src, int i, int c0, int c1, int c2, int c3, int c4,
	int c5, int c6, int c7)
{
  int buf[8];
  FORKS
  __builtin_memcpy (buf, src, sizeof buf);
  return buf[i & 7];
}

/* { dg-final { scan-tree-dump-times "Deleted dead call: __builtin_memset" 1 "dse1" } } */
/* { dg-final { scan-tree-dump-times "Deleted dead store: buf = \\.DEFERRED_INIT" 2 "dse1" } } */
