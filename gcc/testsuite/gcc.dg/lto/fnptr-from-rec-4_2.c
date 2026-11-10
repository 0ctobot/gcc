struct ops
{
  int (*a) (int);
  int (*b) (int);
};

struct obj
{
  const struct ops *ops;
};

static int a2 (int x) { return x + 4; }
static int a3 (int x) { return x + 5; }
static int b3 (int x) { return x + 6; }

const struct ops ops3 = { .a = a2, .b = b3 };
const struct ops ops4 = { .a = a3 };
struct obj o2 = { &ops3 };
