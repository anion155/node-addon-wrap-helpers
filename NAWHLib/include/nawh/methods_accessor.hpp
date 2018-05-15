#pragma once

#include <nawh/accessor_base.hpp>
#include <nawh/method_traits.hpp>

namespace nawh {
namespace __hidden__ {
template <typename _Class, typename _Type>
  using method_getter_type = _Type (_Class::*)();
template <typename _Type, _Type _getter, typename _ConvertType>
  struct methods_accessor_getter;
template <typename _Wrapper, typename _AccessorType, method_getter_type<_Wrapper, _AccessorType> _getter, typename _ConvertType>
  struct methods_accessor_getter<method_getter_type<_Wrapper, _AccessorType>, _getter, _ConvertType> {
    static void get(_Wrapper *holder, std::string property, Nan::NAN_GETTER_ARGS_TYPE info) {
      nawh::UNUSED(property);
      _ConvertType result = (holder->*_getter)();
      info.GetReturnValue().Set(nawh::converter<_ConvertType>::to_value(result));
    }
  };

template <typename _Class, typename _Type>
  using method_setter_type = void (_Class::*)(_Type value);
template <typename _Type, _Type _setter, typename _ConvertType>
  struct methods_accessor_setter;
template <typename _Wrapper, typename _AccessorType, method_setter_type<_Wrapper, _AccessorType> _setter, typename _ConvertType>
  struct methods_accessor_setter<method_setter_type<_Wrapper, _AccessorType>, _setter, _ConvertType> {
    static void set(_Wrapper *holder, std::string property, const v8::Local<v8::Value> &value, Nan::NAN_SETTER_ARGS_TYPE info) {
      nawh::UNUSED(property);
      (holder->*_setter)(nawh::converter<_ConvertType>::to_type(value));
      info.GetReturnValue().SetUndefined();
    }
  };
}
}
