#pragma once

#include <nan.h>
#include <nawh/converters.hpp>
#include <nawh/callbacks_traits.hpp>
#include <nawh/errors.hpp>

namespace nawh {
namespace __hidden__ {
  struct accessor_helper {
    template <typename _Wrapper, typename _PropType>
    static _Wrapper *get_holder(const Nan::PropertyCallbackInfo<_PropType>& info) {
      _Wrapper *holder;
      try {
        holder = nawh::converter<_Wrapper *>::to_type(info.Holder());
      } catch (const nawh::error_argument_type &error) {
        throw nawh::error_reference("Bad `this` type: " + std::string(error.what()));
      }
      return holder;
    }
    static std::string get_property_name(v8::Local<v8::String> property) {
      return nawh::converter<std::string>::to_type(property);
    }
  };
}
}
