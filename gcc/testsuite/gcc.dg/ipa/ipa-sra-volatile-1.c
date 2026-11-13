/* { dg-do compile } */
/* { dg-options "-O2 -fdump-ipa-sra-details -fdump-tree-optimized" } */

/* Each caller reads an aggregate argument from volatile memory.  The callees
   ignore the parameter or use only part of it, but the callers must still
   read the whole argument, so IPA-SRA must neither remove nor split the
   parameter.  */

struct word { unsigned long value; };
struct pair { unsigned first, second; };
struct holder { volatile struct word w; };

volatile struct word vword;
struct holder holder;
int counter;

static __attribute__ ((noinline)) int
ignore_1 (struct word w)
{
  return counter += 1;
}

static __attribute__ ((noinline)) int
ignore_2 (struct word w)
{
  return counter += 2;
}

static __attribute__ ((noinline)) int
ignore_3 (struct word w)
{
  return counter += 3;
}

static __attribute__ ((noinline)) unsigned
first_of_pair (struct pair p)
{
  return p.first;
}

int
read_through_pointer (struct word *p)
{
  return ignore_1 (*(const volatile struct word *) p);
}

int
read_volatile_var (void)
{
  return ignore_2 (vword);
}

int
read_volatile_member (void)
{
  return ignore_3 (holder.w);
}

unsigned
read_partial (struct pair *p)
{
  return first_of_pair (*(const volatile struct pair *) p);
}

/* { dg-final { scan-ipa-dump-times "is loaded from volatile memory, not removing" 4 "sra" } } */
/* { dg-final { scan-tree-dump-not "\\.isra" "optimized" } } */
