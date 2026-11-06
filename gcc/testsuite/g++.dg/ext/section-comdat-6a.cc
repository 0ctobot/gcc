__attribute__((section(".foo"), retain)) inline int a = 1;
__attribute__((section(".foo"))) inline int b = 2;

int *
pa ()
{
  return &a;
}

int *
pb ()
{
  return &b;
}
