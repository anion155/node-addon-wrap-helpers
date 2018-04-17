#include <nawh/object_wrap.hpp>
#include <iostream>

enum B { };

class A : public nawh::object_wrap {
public:
  A() { }
  A(A&&);
  A(int a);
  A(double a);
  A(std::string a);
  A(double a, int b);
  ~A();

  static void class_template(nawh::object_wrap_helper<A> *wrap) {
    wrap
        ->constructor()
        ->constructor<double>()
        ->constructor<int>()
        ->constructor<std::string>()
        ->constructor<double, int>()
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
