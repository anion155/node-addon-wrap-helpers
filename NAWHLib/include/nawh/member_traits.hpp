#pragma once

#include <nawh/type_traits.hpp>
#include <nawh/errors.hpp>
#include <nawh/converters.hpp>
#include <nawh/utilities.hpp>
#include <nawh/accessor_base.hpp>

namespace nawh {
template <typename _MemberRefType> struct member_traits;
template <class _Wrapper, typename _MemberType>
struct member_traits<_MemberType _Wrapper::*> {
  using class_type = _Wrapper;
  using member_type = _MemberType;
  using member_ref = _MemberType _Wrapper::*;
};

template <typename _Class, typename _Type> struct is_member_of : std::false_type { };
template <typename _Class, typename _MemberType>
  struct is_member_of<_Class, _MemberType _Class::*> : std::true_type { };
}
