#include <nawh/object_wrap.hpp>
#include <iostream>

enum B { };

class A : public nawh::object_wrap {
public:
  A() {}
  A(A&&) {}
  A(int a);
  A(double a);
  A(std::string a);
  A(double a, int b);
};

class BC : public A { };

class C {};

NAN_MODULE_INIT(Init) {
  nawh::object_wrap_helper<A>::export_wrap("A", target, [](nawh::object_wrap_helper<A> *wrap) {
    wrap
        ->constructor<double>()
        ->constructor<int>()
        ->constructor<std::string>()
        ->constructor<double, int>()
        ;
  });
  Nan::Set(target, Nan::New("blah").ToLocalChecked(), nawh::handle(new A()));
  static_assert(std::is_constructible<A, double, double>::value, "Can't construct class with provided arguments");

  nawh::object_wrap_helper<BC>::export_wrap<A>("BC", target, [](nawh::object_wrap_helper<BC> *wrap) { });
  Nan::Set(target, Nan::New("blah2").ToLocalChecked(), nawh::handle(new BC()));
}
NODE_MODULE(Blah, Init);
