#pragma once

#include <nan.h>
#include <nawh/converters.hpp>
#include <nawh/callbacks_traits.hpp>

namespace nawh {
namespace __hidden__ {
template <typename _GetterWrapper, typename _SetterWrapper, typename _Wrapper>
  struct accessor_base {
    static NAN_GETTER(getter) {
      _Wrapper *holder;
      try {
        holder = nawh::converter<_Wrapper *>::to_type(info.Holder());
      } catch (const nawh::error_argument_type &error) {
        throw nawh::error_reference("Bad `this` type: " + std::string(error.what()));
      }
      std::string prop_name = nawh::converter<std::string>::to_type(property);
      _GetterWrapper::get(holder, prop_name, info);
    }

    static NAN_SETTER(setter) {
      _Wrapper *holder;
      try {
        holder = nawh::converter<_Wrapper *>::to_type(info.Holder());
      } catch (const nawh::error_argument_type &error) {
        throw nawh::error_reference("Bad `this` type: " + std::string(error.what()));
      }
      std::string prop_name = nawh::converter<std::string>::to_type(property);
      _SetterWrapper::set(holder, prop_name, value, info);
    }
  };
}
}
