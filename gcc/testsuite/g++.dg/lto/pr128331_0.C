// { dg-lto-do link }
// { dg-lto-options { { -O3 -flto -w } } }

// IPA ICF makes the virtual d::p an alias of d::r, so the cached target lists
// name d::r.  Once the alias is removed the inliner reuses d::r as an inline
// clone, and it must not stay in those lists.

class d b(char *) __attribute__((__noreturn__));
void *c;
struct f;
template <typename> struct g {
  g(){}
  [[gnu::noipa]]
  f* h(){}
  g(const g &) { h()->j(); }
  void operator*() {
    if (c)
      b("");
  }
};
struct f {
  virtual f *j() const{}
  virtual void k(d &){}
};
struct m : f {
  g<int> i;
};
struct n : m {
  n *j() const;
  void k(d &);
};
struct o : f {
  g<f> l;
  o(const g<f> &);
  o *j() const;
  void k(d &);
};
struct C {
  virtual void p(o &);
};
struct d : C {
  void p(o &);
  void r(m &);
  void s();
};
void d::r(m &p1) {
  s();
  *p1.i;
  s();
}
void d::p(o &p1) {
  s();
  *p1.l;
  s();
}
o::o(const g<f> &p1) : l(p1) {}
o *o::j() const {}
void o::k(d &p1) { p1.p(*this); }
n *n::j() const { return new n; }
void n::k(d &p1) { p1.r(*this); }
int main()
{
  g<f> t;
  o a(t);
}