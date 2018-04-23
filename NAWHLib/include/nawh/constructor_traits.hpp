#pragma once

#include <nan.h>
#include <utility>
#include <nawh/defines.hpp>
#include <nawh/errors.hpp>
#include <nawh/converters.hpp>

namespace nawh {

namespace __hidden__ {
template <class _Class, typename _seq, typename ..._Args>
  struct constructor_wrapper;
template<class _Class, std::size_t ..._i, typename ..._Args>
  struct constructor_wrapper<_Class, std::integer_sequence<std::size_t, _i...>, _Args...> {
    static _Class *wrapped(Nan::NAN_METHOD_ARGS_TYPE info) {
      if (info.Length() NAWH_ARRAY_INCOMPATIBLE_SIZE_OP sizeof... (_Args)) {
        throw nawh::error_argument_array(sizeof... (_Args));
      }
      return new _Class(nawh::converter<_Args>::to_type(info[_i])...);
    }
  };
}
template <typename _T, class = void> struct constructor_traits;
template <class _Class, typename ..._Args>
struct constructor_traits<_Class(_Args...), typename std::enable_if<std::is_constructible<_Class, _Args...>::value>::type> {
  using class_type = _Class;

  using args_types = std::tuple<_Args...>;
template <std::size_t _n>
  using arg_nth_type = typename std::tuple_element<_n, args_types>::type;

  using alloc_type = std::allocator<_Class>;
  using alloc_traits_type = std::allocator_traits<alloc_type>;

  using wrapper = __hidden__::constructor_wrapper<_Class, std::index_sequence_for<_Args...>, _Args...>;
};
}
