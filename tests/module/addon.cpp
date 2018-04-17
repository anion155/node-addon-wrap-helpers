#include <nawh/object_wrap.hpp>
#include <iostream>

enum B { };

static std::string world() { return "world"; }
static std::string world2() { return "world2"; }

class A : public nawh::object_wrap {
public:
  A() { }
  A(A&&);
  A(int a);
  A(double a);
  A(std::string a);
  A(double a, int b);
  ~A();

  std::string hello(int a) { return "hello " + std::to_string(a); }
  std::string hello2() { return "hello2"; }

  static void class_template(nawh::object_wrap_helper<A> *wrap) {
    wrap
        ->constructor()
        ->constructor<double>()
        ->constructor<int>()
        ->constructor<std::string>()
        ->constructor<double, int>()
        ->method<decltype(&A::hello), &A::hello>("hello")
        ->method<decltype(&A::hello2), &A::hello2>("hello2")
        ->method<decltype(&world), &world>("world")
        ->method<decltype(&world2), &world2>("world2")
        ;
  }
};
A::~A() { }

class BC : public nawh::object_wrap {
public:
  BC() {}
  BC(BC &&);
  ~BC();

  static void class_template(nawh::object_wrap_helper<BC> *wrap) {
    wrap->constructor();
  }
};
BC::~BC() { }

class C {};

NAN_MODULE_INIT(Init) {
  nawh::object_wrap_helper<A>::export_wrap("A", target);
  Nan::Set(target, Nan::New("blah").ToLocalChecked(), nawh::handle(new A()));
  nawh::object_wrap_helper<BC>::export_wrap("BC", target);
  Nan::Set(target, Nan::New("blah2").ToLocalChecked(), nawh::handle(new BC()));
}
NODE_MODULE(Blah, Init);
