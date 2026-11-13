struct word { unsigned long value; };
volatile struct word vword;
volatile int sink;

static __attribute__ ((noinline)) void
g (int i)
{
  sink = i;
}

__attribute__ ((noinline)) void
ignore_word (struct word w, int k)
{
  for (int i = 0; i < k; i++)
    g (i);
}
