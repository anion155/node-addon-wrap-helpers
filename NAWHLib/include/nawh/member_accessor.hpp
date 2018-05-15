#pragma once

#include <nawh/accessor_base.hpp>
#include <nawh/member_traits.hpp>

namespace nawh {
namespace __hidden__ {
template <typename _Wrapper, typename _Type>
  using member_type = _Type _Wrapper::*;

template <typename _Type, _Type _member, typename _ConvertType>
  struct member_accessor;
template <typename _AccessorType, typename _Wrapper, member_type<_Wrapper, _AccessorType> _member, typename _ConvertType>
  struct member_accessor<member_type<_Wrapper, _AccessorType>, _member, _ConvertType> {
    static void get(_Wrapper *holder, std::string property, Nan::NAN_GETTER_ARGS_TYPE info) {
      nawh::UNUSED(property);
      _ConvertType result = (holder->*_member);
      info.GetReturnValue().Set(nawh::converter<_ConvertType>::to_value(result));
    }
    static void set(_Wrapper *holder, std::string property, const v8::Local<v8::Value> &value, Nan::NAN_SETTER_ARGS_TYPE info) {
      nawh::UNUSED(property);
      (holder->*_member) = nawh::converter<_ConvertType>::to_type(value);
      info.GetReturnValue().SetUndefined();
    }
  };
}
}
