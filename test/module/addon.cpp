#include <nawh/object_wrap.hpp>
#include <nawh/converters.hpp>
#include <iostream>

enum B { };

static std::string world() { return "world"; }
static std::string world2(int a) { return "world2 " + std::to_string(a); }

template <typename T>
class Property {
  using field_type = T;
  field_type m_field;

public:
  Property &operator =(const field_type &value) { return set(value); }
  Property &operator =(field_type &&value) { return set(std::forward<field_type>(value)); }

  field_type &get() { return m_field; }
  const field_type &get() const { return m_field; }
  Property &set(const field_type &value) {
    m_field = value;
    return *this;
  }
  Property &set(field_type &&value) {
    m_field = std::forward<field_type>(value);
    return *this;
  }

  operator field_type() { return get(); }
  operator const field_type() const { return get(); }
};

class A : public nawh::object_wrap {
public:
  std::string prop = "default_prop";
  Property<int> prop2;
  A *prop3;
  Property<double> prop4;

  A() { prop2 = 5; prop4 = 10.5; prop3 = this; }
  A(const A&b) { prop3 = const_cast<A*>(&b); }
  A(int a) { prop2 = a; }
  A(double a) { prop4 = a; }
  A(const std::string &a) : prop(a) { }
  A(double a, int b) { prop4 = a; _prop5 = b; }
  ~A();

  std::string hello() { return "hello"; }
  std::string hello2(int a) { return "hello2 " + std::to_string(a); }

  int _prop5 = 53;
  int &get_prop5() { return _prop5; }
  void set_prop5(const int &a) { _prop5 = a; }

  static void class_template(nawh::object_wrap_helper<A> *wrap) {
    wrap
        ->constructor()
        ->constructor<const A &>()
        ->constructor<double>()
        ->constructor<int>()
        ->constructor<std::string>()
        ->constructor<double, int>()

        ->method<decltype (&A::hello), &A::hello>("hello")
        ->method<decltype (&world), &world>("world")
    #ifdef __cpp_template_auto
        ->method<&A::hello2>("hello2")
        ->method<&world2>("world2")
    #else
        ->method<decltype (&A::hello2), &A::hello2>("hello2")
        ->method<decltype (&world2), &world2>("world2")
    #endif
        ->method([]() { return "lambda"; }, "lambda")
        ->method([](int a) { return "lambda2 " + std::to_string(a); }, "lambda2")

        ->accessor<decltype (&A::get_prop5), &A::get_prop5, decltype (&A::set_prop5), &A::set_prop5>("prop5")
        ->accessor<decltype (&A::prop), &A::prop>("prop")
        ->accessor<decltype (&A::prop2), &A::prop2, int>("prop2")
    #ifdef __cpp_template_auto
        ->accessor<&A::prop3>("prop3")
        ->accessor<&A::prop4, double>("prop4")
    #else
        ->accessor<decltype (&A::prop3), &A::prop3>("prop3")
        ->accessor<decltype (&A::prop4), &A::prop4, double>("prop4")
    #endif
        ;
  }
};
A::~A() { }

class BC : public A {
public:
  BC() { }
  BC(BC &&) { }
  ~BC();

  static void class_template(nawh::object_wrap_helper<BC> *wrap) {
    wrap->constructor();
  }
};
BC::~BC() { }

class C { };

NAN_MODULE_INIT(Init) {
  nawh::object_wrap_helper<A>::export_wrap("A", target);
  Nan::Set(target, Nan::New("blah").ToLocalChecked(), nawh::handle(new A()));
  nawh::object_wrap_helper<BC>::export_wrap<A>("BC", target);
  Nan::Set(target, Nan::New("blah2").ToLocalChecked(), nawh::handle(new BC()));
}
NODE_MODULE(Blah, Init);
