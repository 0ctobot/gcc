/* -fno-delete-null-pointer-checks keeps a null check on a parameter that
   the nonnull attribute (or [static N]) says can't be null.  PR101665.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-delete-null-pointer-checks -fdump-tree-optimized" } */

struct node { struct node *next; int v; };
typedef int (*cmp_fn) (void *, struct node *, struct node *);

void fail_head (void);
void fail_cmp (void);
void fail_deref (void);
void fail_static (void);
void fail_inline (void);
void end_loop (void);

__attribute__((nonnull (2, 3))) int
sort_checked (void *priv, struct node *head, cmp_fn cmp)
{
  if (!head)
    fail_head ();
  if (!cmp)
    fail_cmp ();
  return cmp (priv, head->next, head);
}

/* A check after a dereference.  */

__attribute__((nonnull)) int
after_deref (int *p)
{
  int x = *p;
  if (!p)
    fail_deref ();
  return x;
}

/* The loop's first test of P checks the incoming value.  */

__attribute__((nonnull)) int
count (struct node *p)
{
  int c = 0;
  for (; p; p = p->next)
    c++;
  if (c == 0)
    end_loop ();
  return c;
}

/* The C front end synthesizes a nonnull attribute for [static N].  */

int
static_param (int p[static 1])
{
  if (!p)
    fail_static ();
  return p[0];
}

/* A nonnull static inline callee.  */

static inline __attribute__((nonnull)) int
callee (int *p)
{
  if (!p)
    fail_inline ();
  return *p;
}

int
caller (int *p)
{
  return callee (p);
}

/* { dg-final { scan-tree-dump-times "fail_head \\(\\)" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "fail_cmp \\(\\)" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "fail_deref \\(\\)" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "end_loop \\(\\)" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "fail_static \\(\\)" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "fail_inline \\(\\)" 1 "optimized" } } */
