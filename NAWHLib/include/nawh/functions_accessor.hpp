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
    static void get(void *holder, std::string property, Nan::NAN_GETTER_ARGS_TYPE info) {
      nawh::UNUSED(holder, property);
      _ConvertType result = (*_getter)();
      info.GetReturnValue().Set(nawh::converter<_ConvertType>::to_value(result));
    }
  };


template <typename _Type>
  using function_setter_type = void (*)(_Type value);
template <typename _Type, _Type _setter, typename _ConvertType>
  struct functions_accessor_setter;
template <typename _AccessorType, function_setter_type<_AccessorType> _setter, typename _ConvertType>
  struct functions_accessor_setter<function_setter_type<_AccessorType>, _setter, _ConvertType> {
    static void set(void *holder, std::string property, const v8::Local<v8::Value> &value, Nan::NAN_SETTER_ARGS_TYPE info) {
      nawh::UNUSED(holder, property);
      (*_setter)(nawh::converter<_ConvertType>::to_type(value));
      info.GetReturnValue().SetUndefined();
    }
  };

template <typename _Wrapper, typename _Type>
  using function_this_getter_type = _Type (*)(_Wrapper *that);
template <typename _Type, _Type _getter, typename _ConvertType>
  struct functions_this_accessor_getter;
template <typename _Wrapper, typename _AccessorType, function_this_getter_type<_Wrapper, _AccessorType> _getter, typename _ConvertType>
  struct functions_this_accessor_getter<function_this_getter_type<_Wrapper, _AccessorType>, _getter, _ConvertType> {
    static void get(void *holder, std::string property, Nan::NAN_GETTER_ARGS_TYPE info) {
      nawh::UNUSED(holder, property);
      _ConvertType result = (*_getter)(holder);
      info.GetReturnValue().Set(nawh::converter<_ConvertType>::to_value(result));
    }
  };


template <typename _Wrapper, typename _Type>
  using function_this_setter_type = void (*)(_Wrapper *that, _Type value);
template <typename _Type, _Type _setter, typename _ConvertType>
  struct functions_this_accessor_setter;
template <typename _Wrapper, typename _AccessorType, function_this_setter_type<_Wrapper, _AccessorType> _setter, typename _ConvertType>
  struct functions_this_accessor_setter<function_this_setter_type<_Wrapper, _AccessorType>, _setter, _ConvertType> {
    static void set(void *holder, std::string property, const v8::Local<v8::Value> &value, Nan::NAN_SETTER_ARGS_TYPE info) {
      nawh::UNUSED(holder, property);
      (*_setter)(holder, nawh::converter<_ConvertType>::to_type(value));
      info.GetReturnValue().SetUndefined();
    }
  };
}
}
