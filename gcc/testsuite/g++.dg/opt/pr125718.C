/* { dg-do compile } */
/* { dg-options "-O3 -fnon-call-exceptions -fno-strict-aliasing -std=c++20" } */

template <typename a> struct b {
  using c = a;
};
template <typename aa, template <typename> class> using d = b<aa>::c;
template <typename, typename> struct e;
template <template <typename> class f, typename g, typename h, typename... i>
struct e<f<h, i...>, g> {
  using c = f<g>;
};
template <typename> struct l;
template <typename h> struct l<h *> {
  using j = h;
};
template <typename k> class n {
public:
  n(k);
  l<k>::j operator*();
};
template <typename h> class o {
public:
  typedef h m;
};
struct ab {
  template <typename h, typename g> struct ac : e<h, g> {};
  template <typename h> using p = h;
};
template <typename ad, typename g> using q = ab::ac<ad, g>::c;
template <typename ad> struct r : ab {
  using af = d<typename ad::m *, p>;
  struct ag {
    using c = int;
  };
  using ah = ag::c;
  template <typename h> using ai = q<ad, h>;
  static void ae(ad, af, ah);
};
template <typename ad> struct aj : r<ad> {
  typedef ad::m j;
  template <typename h> struct ak {
    typedef r<ad>::template ai<h> al;
  };
};
class am {
  friend bool operator==(am, am);
};
class w {
public:
  typedef am an;
  an ao();
};
template <typename h, typename ad> struct x {
  typedef aj<ad>::template ak<h>::al ap;
  typedef aj<ap>::af af;
  struct y {
    af aq;
    af ar;
    y() : aq(), ar() {}
  };
  struct as : ap, y {};
  ~x() { au(); }
  as av;
  af aw;
  long ax;
  void au() { aj<ap>::ae(av, aw, ax); }
};
template <typename h, typename ad = o<h>> class ay : x<h, ad> {
  typedef x<h, ad> s;

public:
  typedef aj<typename s::ap>::j j;
  n<typename s::af> ao() { return this->av.ar; }
  long az() {
    long t = this->av.ar - this->av.aq;
    if (t)
      __builtin_unreachable();
    return t;
  }
  j operator[](long) { return *this->av.aq; }
  long ba;
  void bb() {
    if (ba >= az())
      throw;
  }
  j at(long bc) {
    bb();
    return (*this)[bc];
  }
  j back() { return *ao(); }
  template <typename...> constexpr j u();
};
template <typename h, typename ad>
template <typename...>
constexpr ay<h, ad>::j ay<h, ad>::u() {
  ++this->av.ar;
  return back();
}
struct bd v(ay<bd> &);
struct bd {
  int *be;
};
struct bf {
  int *bg[8];
  bd *z;
};
w bh;
void bi(bf &bc) {
  ay<int> bj;
  ay<bd> bk;
  for (w::an bl; bl != bh.ao();)
    bj.u();
  v(bk);
  bk.at(0);
  for (; bc.z;) {
    long bm = bc.z[0].be[0], bn = bk.at(0).be[0];
    bc.bg[0][bc.z[0].be[0]] = bj.at(bn);
  }
}
