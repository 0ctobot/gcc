struct ops
{
  int (*a) (int);
  int (*b) (int);
};

struct obj
{
  const struct ops *ops;
};

static int a1 (int x) { return x + 1; }
static int b1 (int x) { return x + 2; }
static int b2 (int x) { return x + 3; }

const struct ops ops1 = { .a = a1, .b = b1 };
const struct ops ops2 = { .b = b2 };
struct obj o1 = { &ops1 };
