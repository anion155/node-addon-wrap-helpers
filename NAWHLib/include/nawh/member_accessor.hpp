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
    static NAN_GETTER(getter) NAWH_TRY {
      nawh::UNUSED(property);
      _Wrapper *holder = accessor_helper::get_holder<_Wrapper>(info);
      _ConvertType result = (holder->*_member);
      info.GetReturnValue().Set(nawh::converter<_ConvertType>::to_value(result));
    } NAWH_CATCH
    static NAN_SETTER(setter) NAWH_TRY {
      nawh::UNUSED(property);
      _Wrapper *holder = accessor_helper::get_holder<_Wrapper>(info);
      (holder->*_member) = nawh::converter<_ConvertType>::to_type(value);
      info.GetReturnValue().SetUndefined();
    } NAWH_CATCH
  };
}
}
