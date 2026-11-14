struct p { int x, y; };

const struct p w = { 70, 80 };

extern int f_base (int);

const struct p *
get_w (void)
{
  return &w;
}

int
f (int x)
{
  return x > 0 ? f_base (x - 1) + 10 : -1;
}
