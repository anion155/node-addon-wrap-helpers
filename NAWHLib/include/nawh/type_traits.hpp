#pragma once

#include <type_traits>
#include <nan.h>

namespace nawh {

template <template <typename ...> class _Templ, typename _Spec>
struct is_specialization_of : std::false_type { };
template <template <typename ...> class _Templ, typename ..._Args>
struct is_specialization_of<_Templ, _Templ<_Args...>> : std::true_type { };

template <class, bool> struct object_wrap_helper;
namespace __hidden__ {
  struct do_has_class_template {
  template <class _Wrapper, typename = decltype(static_cast<void(*)(nawh::object_wrap_helper<_Wrapper, true> *)>(&_Wrapper::class_template))>
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
namespace __hidden__ {
  struct do_has_converter {
  template <class _Type, typename = decltype(&nawh::converter<_Type, void>::to_type)>
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

namespace __hidden__ {
  struct do_is_functor {
  template <class _Type, typename = decltype(&_Type::operator())>
    static std::true_type __test(int);
  template <typename>
    static std::false_type __test(...);
  };
template <class _Type>
  struct is_functor : public do_is_functor {
    typedef decltype(__test<_Type>(0)) type;
  };
}
template <class _Type>
struct is_functor
    : public __hidden__::is_functor<_Type>::type
{ };

namespace __hidden__ {
  struct do_can_convert_to_bool {
  template <class _Type, typename = decltype(&_Type::operator bool)>
    static std::true_type __test(int);
  template <typename>
    static std::false_type __test(...);
  };
template <class _Type>
  struct can_convert_to_bool : public do_can_convert_to_bool {
    typedef decltype(__test<_Type>(0)) type;
  };
}
template <class _Type>
struct can_convert_to_bool
    : public __hidden__::can_convert_to_bool<_Type>::type
{ };

template<class _Functor>
struct is_lambda
  : std::integral_constant<bool, is_functor<_Functor>::value && !can_convert_to_bool<_Functor>::value>
{ };
}
