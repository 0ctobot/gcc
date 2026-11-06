/* { dg-do compile } */

/* Deleting a call that can't return leaves a block with no successor edges,
   which crashed ipa-fnsummary.  */

typedef __SIZE_TYPE__ size_t;

extern void *memset (void *, int, size_t) __attribute__((noreturn));
extern void *__memset_chk (void *, int, size_t, size_t)
  __attribute__((noreturn));
extern void free (void *) __attribute__((noreturn));
extern void *malloc (size_t);
extern void *calloc (size_t, size_t);

struct s { int a[4]; };

int g;
void h (void);

void
f1 (char *p)
{
  *p = 0;
  if (g)
    memset (p, 0, 1);
}

char *
f2 (char *p)
{
  *p = 0;
  if (g)
    return memset (p, 0, 1);
  return p;
}

void
f3 (struct s *p)
{
  *p = (struct s) { 0 };
  if (g)
    memset (p, 0, sizeof *p);
  else
    h ();
}

void
f4 (char *p, int n)
{
  *p = 0;
  for (int i = 0; i < n; i++)
    if (p[i + 1])
      memset (p, 0, 1);
}

void
f5 (char *p)
{
  *p = 0;
  if (g)
    __memset_chk (p, 0, 1, __builtin_object_size (p, 0));
}

void
f6 (size_t n)
{
  void *p = malloc (n);
  if (g)
    free (p);
  else
    h ();
}

char *
f7 (void)
{
  char *p = calloc (8, 1);
  if (g)
    memset (p, 0, 8);
  else
    h ();
  return p;
}
