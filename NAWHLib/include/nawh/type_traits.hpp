#pragma once

#include <type_traits>
#include <nan.h>

namespace nawh {

template <class, bool> struct object_wrap_helper;
namespace __hidden__ {
  struct do_has_class_template {
  template <class _Wrapper, typename = decltype(_Wrapper::class_template(nawh::object_wrap_helper<_Wrapper, true>::_wrap))>
    static std::true_type __test(int);
  template <typename>
    static std::false_type __test(...);
  };
template <class _Wrapper>
  struct has_class_template : public do_has_class_template {
    typedef decltype(__test<_Wrapper>(0)) type;
  };
}
template <class _Wrapper>
struct has_class_template
    : public __hidden__::has_class_template<_Wrapper>::type
{ };

template <typename, typename> struct converter;
template <class, bool> struct object_wrap_helper;
namespace __hidden__ {
  struct do_has_converter {
  template <class _Type, typename = decltype(nawh::converter<_Type, void>::to_type(v8::Local<v8::Value>()))>
    static std::true_type __test(int);
  template <typename>
    static std::false_type __test(...);
  };
template <class _Type>
  struct has_converter : public do_has_converter {
    typedef decltype(__test<_Type>(0)) type;
  };
}
template <class _Type>
struct has_converter
    : public __hidden__::has_converter<_Type>::type
{ };
}