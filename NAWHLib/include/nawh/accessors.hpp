#pragma once

#include <nawh/accessor_base.hpp>
#include <nawh/functions_accessor.hpp>
#include <nawh/methods_accessor.hpp>
#include <nawh/member_accessor.hpp>

namespace nawh {
namespace __hidden__ {
template <typename _Type, _Type _getter, typename _ConvertType> struct accessor_getter;
template <typename _Type, _Type _setter, typename _ConvertType> struct accessor_setter;

template <typename _AccessorType, function_getter_type<_AccessorType> _getter, typename _ConvertType>
  struct accessor_getter<function_getter_type<_AccessorType>, _getter, _ConvertType>
      : functions_accessor_getter<function_getter_type<_AccessorType>, _getter, _ConvertType>
  { };
template <typename _AccessorType, function_setter_type<_AccessorType> _setter, typename _ConvertType>
  struct accessor_setter<function_setter_type<_AccessorType>, _setter, _ConvertType>
      : functions_accessor_setter<function_setter_type<_AccessorType>, _setter, _ConvertType>
  { };

template <typename _Wrapper, typename _AccessorType, function_this_getter_type<_Wrapper, _AccessorType> _getter, typename _ConvertType>
  struct accessor_getter<function_this_getter_type<_Wrapper, _AccessorType>, _getter, _ConvertType>
      : functions_this_accessor_getter<function_this_getter_type<_Wrapper, _AccessorType>, _getter, _ConvertType>
  { };
template <typename _Wrapper, typename _AccessorType, function_this_getter_type<_Wrapper, _AccessorType> _setter, typename _ConvertType>
  struct accessor_setter<function_this_getter_type<_Wrapper, _AccessorType>, _setter, _ConvertType>
      : functions_this_accessor_setter<function_this_getter_type<_Wrapper, _AccessorType>, _setter, _ConvertType>
  { };

template <typename _Wrapper, typename _AccessorType, member_type<_Wrapper, _AccessorType> _member, typename _ConvertType>
  struct accessor_getter<member_type<_Wrapper, _AccessorType>, _member, _ConvertType>
      : member_accessor<member_type<_Wrapper, _AccessorType>, _member, _ConvertType>
  { };
template <typename _Wrapper, typename _AccessorType, member_type<_Wrapper, _AccessorType> _member, typename _ConvertType>
  struct accessor_setter<member_type<_Wrapper, _AccessorType>, _member, _ConvertType>
      : member_accessor<member_type<_Wrapper, _AccessorType>, _member, _ConvertType>
  { };

template <typename _Wrapper, typename _AccessorType, method_getter_type<_Wrapper, _AccessorType> _getter, typename _ConvertType>
  struct accessor_getter<method_getter_type<_Wrapper, _AccessorType>, _getter, _ConvertType>
      : methods_accessor_getter<method_getter_type<_Wrapper, _AccessorType>, _getter, _ConvertType>
  { };
template <typename _Wrapper, typename _AccessorType, method_setter_type<_Wrapper, _AccessorType> _setter, typename _ConvertType>
  struct accessor_setter<method_setter_type<_Wrapper, _AccessorType>, _setter, _ConvertType>
      : methods_accessor_setter<method_setter_type<_Wrapper, _AccessorType>, _setter, _ConvertType>
  { };
}
template <typename _Type, _Type _getter, typename _ConvertType>
  struct accessor_getter : __hidden__::accessor_getter<_Type, _getter, _ConvertType> { };
template <typename _Type, _Type _setter, typename _ConvertType>
  struct accessor_setter : __hidden__::accessor_setter<_Type, _setter, _ConvertType> { };


namespace __hidden__ {
template <typename _Type> struct is_accessor_getter_type : std::false_type { };
template <typename _Type>
struct is_accessor_getter_type<__hidden__::function_getter_type<_Type>>
    : std::true_type
{ };
template <typename _Wrapper, typename _Type>
struct is_accessor_getter_type<__hidden__::function_this_getter_type<_Wrapper, _Type>>
    : std::true_type
{ };
template <typename _Wrapper, typename _Type>
struct is_accessor_getter_type<__hidden__::member_type<_Wrapper, _Type>>
    : std::true_type
{ };
template <typename _Wrapper, typename _Type>
struct is_accessor_getter_type<__hidden__::method_getter_type<_Wrapper, _Type>>
    : std::true_type
{ };

template <typename _Type> struct is_accessor_setter_type : std::false_type { };
template <typename _Type>
struct is_accessor_setter_type<__hidden__::function_setter_type<_Type>>
    : std::true_type
{ };
template <typename _Wrapper, typename _Type>
struct is_accessor_setter_type<__hidden__::function_this_setter_type<_Wrapper, _Type>>
    : std::true_type
{ };
template <typename _Wrapper, typename _Type>
struct is_accessor_setter_type<__hidden__::member_type<_Wrapper, _Type>>
    : std::true_type
{ };
template <typename _Wrapper, typename _Type>
struct is_accessor_setter_type<__hidden__::method_setter_type<_Wrapper, _Type>>
    : std::true_type
{ };
}


namespace __hidden__ {
template <typename _Type> struct accessor_getter_type;
template <typename _Type> struct accessor_setter_type;

template <typename _AccessorType>
  struct accessor_getter_type<function_getter_type<_AccessorType>> {
    using type = _AccessorType;
  };
template <typename _AccessorType>
  struct accessor_setter_type<function_setter_type<_AccessorType>> {
    using type = _AccessorType;
  };

template <typename _Wrapper, typename _AccessorType>
  struct accessor_getter_type<function_this_getter_type<_Wrapper, _AccessorType>> {
    using type = _AccessorType;
  };
template <typename _Wrapper, typename _AccessorType>
  struct accessor_setter_type<function_this_setter_type<_Wrapper, _AccessorType>> {
    using type = _AccessorType;
  };

template <typename _Wrapper, typename _AccessorType>
  struct accessor_getter_type<member_type<_Wrapper, _AccessorType>> {
    using type = _AccessorType;
  };
template <typename _Wrapper, typename _AccessorType>
  struct accessor_setter_type<member_type<_Wrapper, _AccessorType>> {
    using type = _AccessorType;
  };

template <typename _Wrapper, typename _AccessorType>
  struct accessor_getter_type<method_getter_type<_Wrapper, _AccessorType>> {
    using type = _AccessorType;
  };
template <typename _Wrapper, typename _AccessorType>
  struct accessor_setter_type<method_setter_type<_Wrapper, _AccessorType>> {
    using type = _AccessorType;
  };
}
template <typename _GetterType, typename _SetterType>
struct is_accessor_types
    : nawh::__and_<
      __hidden__::is_accessor_getter_type<_GetterType>, __hidden__::is_accessor_setter_type<_SetterType>,
      std::is_same<
        typename std::decay<typename __hidden__::accessor_getter_type<_GetterType>::type>::type,
        typename std::decay<typename __hidden__::accessor_setter_type<_SetterType>::type>::type
      >
    >
{ };

template <typename _GetterType, typename _SetterType>
struct accessor_type
    : std::enable_if<
      nawh::is_accessor_types<_GetterType, _SetterType>::value
      , typename std::decay<typename __hidden__::accessor_getter_type<_GetterType>::type>::type
    >
{ };
}
