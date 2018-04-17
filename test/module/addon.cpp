#include <nawh/object_wrap.hpp>
#include <iostream>

enum B { };

static std::string world() { return "world"; }
static std::string world2(int a) { return "world2 " + std::to_string(a); }

class A : public nawh::object_wrap {
public:
  A() { }
  A(A&&) { }
  A(int) { }
  A(double) { }
  A(std::string) { }
  A(double, int) { }
  ~A();

  std::string hello() { return "hello"; }
  std::string hello2(int a) { return "hello2 " + std::to_string(a); }

  static void class_template(nawh::object_wrap_helper<A> *wrap) {
    wrap
        ->constructor()
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
        ->method_lambda([]() { return "lambda"; }, "lambda")
        ->method_lambda([](int a) { return "lambda2 " + std::to_string(a); }, "lambda2")
        ;
  }
};
A::~A() { }

class BC : public nawh::object_wrap {
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
  nawh::object_wrap_helper<BC>::export_wrap("BC", target);
  Nan::Set(target, Nan::New("blah2").ToLocalChecked(), nawh::handle(new BC()));
}
NODE_MODULE(Blah, Init);
