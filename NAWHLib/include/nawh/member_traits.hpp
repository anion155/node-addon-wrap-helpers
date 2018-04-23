#pragma once

#include <nawh/type_traits.hpp>
#include <nawh/errors.hpp>
#include <nawh/converters.hpp>
#include <nawh/utilities.hpp>

namespace nawh {

namespace __hidden__ {
template <typename _Wrapper, typename _MemberType, typename _PropertyType>
  struct member_property {
    using getter_type = NAN_GETTER();
    template <_MemberType _Wrapper::*_member>
    static NAN_GETTER(getter) {
      nawh::UNUSED(property);
      _Wrapper *holder;
      try {
        holder = nawh::converter<_Wrapper *>::to_type(info.Holder());
      } catch (const nawh::error_argument_type &error) {
        throw nawh::error_reference("Bad `this` type: " + std::string(error.what()));
      }
      _PropertyType value = (holder->*_member);
      info.GetReturnValue().Set(nawh::converter<_PropertyType>::to_value(value));
    }

    using setter_type = NAN_SETTER();
    template <_MemberType _Wrapper::*_member>
    static NAN_SETTER(setter) {
      nawh::UNUSED(property);
      _Wrapper *holder;
      try {
        holder = nawh::converter<_Wrapper *>::to_type(info.Holder());
      } catch (const nawh::error_argument_type &error) {
        throw nawh::error_reference("Bad `this` type: " + std::string(error.what()));
      }
      (holder->*_member) = nawh::converter<_PropertyType>::to_type(value);
    }
  };
}

template <typename _MemberRefType> struct member_traits;
template <class _Wrapper, typename _MemberType>
struct member_traits<_MemberType _Wrapper::*> {
  using class_type = _Wrapper;
  using member_type = _MemberType;
  using member_ref = _MemberType _Wrapper::*;

template <typename _PropertyType = _MemberType>
  using wrapper = typename std::enable_if<
    !std::is_same<typename std::decay<_PropertyType>::type, void>::value
    , __hidden__::member_property<_Wrapper, _MemberType, _PropertyType>
  >::type;
};
}
