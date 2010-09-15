// RUN: %clang_cc1 -fsyntax-only -verify %s 
class X { };

X operator+(X, X);

void f(X x) {
  x = x + x;
}

struct Y;
struct Z;

struct Y {
  Y(const Z&);
};

struct Z {
  Z(const Y&);
};

Y operator+(Y, Y);
bool operator-(Y, Y); // expected-note{{candidate function}}
bool operator-(Z, Z); // expected-note{{candidate function}}

void g(Y y, Z z) {
  y = y + z;
  bool b = y - z; // expected-error{{use of overloaded operator '-' is ambiguous; candidates are:}}
}

struct A {
  bool operator==(Z&); // expected-note 2{{candidate function}}
};

A make_A();

bool operator==(A&, Z&); // expected-note 2{{candidate function}}

void h(A a, const A ac, Z z) {
  make_A() == z;
  a == z; // expected-error{{use of overloaded operator '==' is ambiguous; candidates are:}}
  ac == z; // expected-error{{invalid operands to binary expression ('A const' and 'Z')}}
}

struct B {
  bool operator==(const B&) const;

  void test(Z z) {
    make_A() == z;
  }
};

// we shouldn't see warnings about self-comparison,
// this is a member function, we dunno what it'll do
bool i(B b)
{
  return b == b;
}

enum Enum1 { };
enum Enum2 { };

struct E1 {
  E1(Enum1) { }
};

struct E2 {
  E2(Enum2);
};

// C++ [over.match.oper]p3 - enum restriction.
float& operator==(E1, E2); 

void enum_test(Enum1 enum1, Enum2 enum2, E1 e1, E2 e2) {
  float &f1 = (e1 == e2);
  float &f2 = (enum1 == e2); 
  float &f3 = (e1 == enum2); 
  float &f4 = (enum1 == enum2);  // expected-error{{non-const lvalue reference to type 'float' cannot bind to a temporary of type 'bool'}}
}

// PR5244 - Argument-dependent lookup would include the two operators below,
// which would break later assumptions and lead to a crash.
class pr5244_foo
{
  pr5244_foo(int);
  pr5244_foo(char);
};

bool operator==(const pr5244_foo& s1, const pr5244_foo& s2);
bool operator==(char c, const pr5244_foo& s);

enum pr5244_bar
{
    pr5244_BAR
};

class pr5244_baz
{
public:
    pr5244_bar quux;
};

void pr5244_barbaz()
{
  pr5244_baz quuux;
  (void)(pr5244_BAR == quuux.quux);
}



struct PostInc {
  PostInc operator++(int);
  PostInc& operator++();
};

struct PostDec {
  PostDec operator--(int);
  PostDec& operator--();
};

void incdec_test(PostInc pi, PostDec pd) {
  const PostInc& pi1 = pi++;
  const PostDec& pd1 = pd--;
  PostInc &pi2 = ++pi;
  PostDec &pd2 = --pd;
}

struct SmartPtr {
  int& operator*();
  long& operator*() const volatile;
};

void test_smartptr(SmartPtr ptr, const SmartPtr cptr, 
                   const volatile SmartPtr cvptr) {
  int &ir = *ptr;
  long &lr = *cptr;
  long &lr2 = *cvptr;
}


struct ArrayLike {
  int& operator[](int);
};

void test_arraylike(ArrayLike a) {
  int& ir = a[17];
}

struct SmartRef {
  int* operator&();
};

void test_smartref(SmartRef r) {
  int* ip = &r;
}

bool& operator,(X, Y);

void test_comma(X x, Y y) {
  bool& b1 = (x, y);
  X& xr = (x, x); // expected-warning {{expression result unused}}
}

struct Callable {
  int& operator()(int, double = 2.71828); // expected-note{{candidate function}}
  float& operator()(int, double, long, ...); // expected-note{{candidate function}}

  double& operator()(float); // expected-note{{candidate function}}
};

struct Callable2 {
  int& operator()(int i = 0);
  double& operator()(...) const;
};

struct DerivesCallable : public Callable {
};

void test_callable(Callable c, Callable2 c2, const Callable2& c2c,
                   DerivesCallable dc) {
  int &ir = c(1);
  float &fr = c(1, 3.14159, 17, 42);

  c(); // expected-error{{no matching function for call to object of type 'Callable'}}

  double &dr = c(1.0f);

  int &ir2 = c2();
  int &ir3 = c2(1);
  double &fr2 = c2c();
  
  int &ir4 = dc(17);
  double &fr3 = dc(3.14159f);
}

typedef float FLOAT;
typedef int& INTREF;
typedef INTREF Func1(FLOAT, double);
typedef float& Func2(int, double);

struct ConvertToFunc {
  operator Func1*(); // expected-note 2{{conversion candidate of type 'INTREF (*)(FLOAT, double)'}}
  operator Func2&(); // expected-note 2{{conversion candidate of type 'float &(&)(int, double)'}}
  void operator()();
};

struct ConvertToFuncDerived : ConvertToFunc { };

void test_funcptr_call(ConvertToFunc ctf, ConvertToFuncDerived ctfd) {
  int &i1 = ctf(1.0f, 2.0);
  float &f1 = ctf((short int)1, 1.0f);
  ctf((long int)17, 2.0); // expected-error{{call to object of type 'ConvertToFunc' is ambiguous}}
  ctf();

  int &i2 = ctfd(1.0f, 2.0);
  float &f2 = ctfd((short int)1, 1.0f);
  ctfd((long int)17, 2.0); // expected-error{{call to object of type 'ConvertToFuncDerived' is ambiguous}}
  ctfd();
}

struct HasMember {
  int m;
};

struct Arrow1 {
  HasMember* operator->();
};

struct Arrow2 {
  Arrow1 operator->(); // expected-note{{candidate function}}
};

void test_arrow(Arrow1 a1, Arrow2 a2, const Arrow2 a3) {
  int &i1 = a1->m;
  int &i2 = a2->m;
  a3->m; // expected-error{{no viable overloaded 'operator->'; candidate is}}
}

struct CopyConBase {
};

struct CopyCon : public CopyConBase {
  CopyCon(const CopyConBase &Base);

  CopyCon(const CopyConBase *Base) {
    *this = *Base;
  }
};

namespace N {
  struct X { };
}

namespace M {
  N::X operator+(N::X, N::X);
}

namespace M {
  void test_X(N::X x) {
    (void)(x + x);
  }
}

struct AA { bool operator!=(AA&); };
struct BB : AA {};
bool x(BB y, BB z) { return y != z; }


struct AX { 
  AX& operator ->();	 // expected-note {{declared here}}
  int b;
}; 

void m() {
  AX a; 
  a->b = 0; // expected-error {{circular pointer delegation detected}}
}

struct CircA {
  struct CircB& operator->(); // expected-note {{declared here}}
  int val;
};
struct CircB {
  struct CircC& operator->(); // expected-note {{declared here}}
};
struct CircC {
  struct CircA& operator->(); // expected-note {{declared here}}
};

void circ() {
  CircA a;
  a->val = 0; // expected-error {{circular pointer delegation detected}}
}

// PR5360: Arrays should lead to built-in candidates for subscript.
typedef enum {
  LastReg = 23,
} Register;
class RegAlloc {
  int getPriority(Register r) {
    return usepri[r];
  }
  int usepri[LastReg + 1];
};

// PR5546: Don't generate incorrect and ambiguous overloads for multi-level
// arrays.
namespace pr5546
{
  enum { X };
  extern const char *const sMoveCommands[][2][2];
  const char* a() { return sMoveCommands[X][0][0]; }
  const char* b() { return (*(sMoveCommands+X))[0][0]; }
}

// PR5512 and its discussion
namespace pr5512 {
  struct Y {
    operator short();
    operator float();
  };
  void g_test(Y y) {
    short s = 0;
    // DR507, this should be ambiguous, but we special-case assignment
    s = y;
    // Note: DR507, this is ambiguous as specified
    //s += y;
  }

  struct S {};
  void operator +=(int&, S);
  void f(S s) {
    int i = 0;
    i += s;
  }

  struct A {operator int();};
  int a;
  void b(A x) {
    a += x;
  }
}

// PR5900
namespace pr5900 {
  struct NotAnArray {};
  void test0() {
    NotAnArray x;
    x[0] = 0; // expected-error {{does not provide a subscript operator}}
  }

  struct NonConstArray {
    int operator[](unsigned); // expected-note {{candidate}}
  };
  int test1() {
    const NonConstArray x = NonConstArray();
    return x[0]; // expected-error {{no viable overloaded operator[] for type}}
  }

  // Not really part of this PR, but implemented at the same time.
  struct NotAFunction {};
  void test2() {
    NotAFunction x;
    x(); // expected-error {{does not provide a call operator}}
  }
}

// Operator lookup through using declarations.
namespace N {
  struct X2 { };
}

namespace N2 {
  namespace M {
    namespace Inner {
      template<typename T>
      N::X2 &operator<<(N::X2&, const T&);
    }
    using Inner::operator<<;
  }
}

void test_lookup_through_using() {
  using namespace N2::M;
  N::X2 x;
  x << 17;
}
