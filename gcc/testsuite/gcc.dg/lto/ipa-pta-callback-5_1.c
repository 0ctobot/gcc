__attribute__ ((noinline)) int
parse (const char *s, unsigned char *res)
{
  if (!s[0])
    return -1;
  *res = s[0];
  return 0;
}
