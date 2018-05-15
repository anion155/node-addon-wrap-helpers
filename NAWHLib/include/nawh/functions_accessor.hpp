#pragma once

#include <nawh/accessor_base.hpp>
#include <nawh/function_traits.hpp>

namespace nawh {
namespace __hidden__ {
template <typename _Type>
  using function_getter_type = _Type (*)();
template <typename _Type, _Type _getter, typename _ConvertType>
  struct functions_accessor_getter;
template <typename _AccessorType, function_getter_type<_AccessorType> _getter, typename _ConvertType>
  struct functions_accessor_getter<function_getter_type<_AccessorType>, _getter, _ConvertType> {
    static NAN_GETTER(getter) NAWH_TRY {
      nawh::UNUSED(property);
      _ConvertType result = (*_getter)();
      info.GetReturnValue().Set(nawh::converter<_ConvertType>::to_value(result));
    } NAWH_CATCH
  };


template <typename _Type>
  using function_setter_type = void (*)(_Type value);
template <typename _Type, _Type _setter, typename _ConvertType>
  struct functions_accessor_setter;
template <typename _AccessorType, function_setter_type<_AccessorType> _setter, typename _ConvertType>
  struct functions_accessor_setter<function_setter_type<_AccessorType>, _setter, _ConvertType> {
    static NAN_SETTER(setter) NAWH_TRY {
      nawh::UNUSED(property);
      (*_setter)(nawh::converter<_ConvertType>::to_type(value));
      info.GetReturnValue().SetUndefined();
    } NAWH_CATCH
  };

template <typename _Wrapper, typename _Type>
  using function_this_getter_type = _Type (*)(_Wrapper *that);
template <typename _Type, _Type _getter, typename _ConvertType>
  struct functions_this_accessor_getter;
template <typename _Wrapper, typename _AccessorType, function_this_getter_type<_Wrapper, _AccessorType> _getter, typename _ConvertType>
  struct functions_this_accessor_getter<function_this_getter_type<_Wrapper, _AccessorType>, _getter, _ConvertType> {
    static NAN_GETTER(getter) NAWH_TRY {
      nawh::UNUSED(property);
      _ConvertType result = (*_getter)(accessor_helper::get_holder<_Wrapper>(info));
      info.GetReturnValue().Set(nawh::converter<_ConvertType>::to_value(result));
    } NAWH_CATCH
  };


template <typename _Wrapper, typename _Type>
  using function_this_setter_type = void (*)(_Wrapper *that, _Type value);
template <typename _Type, _Type _setter, typename _ConvertType>
  struct functions_this_accessor_setter;
template <typename _Wrapper, typename _AccessorType, function_this_setter_type<_Wrapper, _AccessorType> _setter, typename _ConvertType>
  struct functions_this_accessor_setter<function_this_setter_type<_Wrapper, _AccessorType>, _setter, _ConvertType> {
    static NAN_SETTER(setter) NAWH_TRY {
      nawh::UNUSED(property);
      (*_setter)(accessor_helper::get_holder<_Wrapper>(info), nawh::converter<_ConvertType>::to_type(value));
      info.GetReturnValue().SetUndefined();
    } NAWH_CATCH
  };
}
}
