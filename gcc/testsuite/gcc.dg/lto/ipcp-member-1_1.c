struct opaque { int z; };
struct inner { int x; long y; };
struct root { struct opaque *o[2]; struct inner cgrp; int n; };
struct root gr;

__attribute__ ((noinline)) long
drain (struct inner *c)
{
  return c->x + c->y;
}

long
use_b (void)
{
  return drain (&gr.cgrp);
}
