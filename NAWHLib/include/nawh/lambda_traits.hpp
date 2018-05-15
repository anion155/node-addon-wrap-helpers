#pragma once

#include <nan.h>
#include <nawh/type_traits.hpp>
#include <nawh/defines.hpp>
#include <nawh/errors.hpp>
#include <nawh/converters.hpp>
#include <nawh/function_traits.hpp>

namespace nawh {


namespace __hidden__ {
  using nan_function_type = NAN_METHOD();

template <typename _Functor, typename _Return, typename _sequence, typename ..._Args>
  struct lambda_traits_impl;
template <typename _Functor, typename _Return, std::size_t ..._i, typename ..._Args>
  struct lambda_traits_impl<_Functor, _Return, std::integer_sequence<std::size_t, _i...>, _Args...> {
    using function_type = _Return(_Args...);
    using function_ref = function_type *;

    static inline function_ref wrap_lambda_to_callback(_Functor lambda) {
        static _Functor _lambda = lambda;
        return [](_Args ...args) { return _lambda(std::forward<_Args>(args)...); };
    }
    static __hidden__::nan_function_type *wrap_lambda_to_nan(_Functor lambda) {
      static _Functor _lambda = lambda;
      return [](Nan::NAN_METHOD_ARGS_TYPE info) -> Nan::NAN_METHOD_RETURN_TYPE NAWH_TRY {
        if (info.Length() NAWH_ARRAY_INCOMPATIBLE_SIZE_OP sizeof...(_Args)) {
          throw nawh::error_argument_array(sizeof...(_Args));
        }
        if constexpr (std::is_same<_Return, void>::value) {
          _lambda(nawh::converter<_Args>::to_type(info[_i])...);
          info.GetReturnValue().SetUndefined();
        } else {
          auto result = _lambda(nawh::converter<_Args>::to_type(info[_i])...);
          info.GetReturnValue().Set(nawh::converter<_Return>::to_value(result));
        }
      } NAWH_CATCH;
    }
  };
}

template <typename _Functor> struct lambda_traits : lambda_traits<decltype (&_Functor::operator())> {};
template <typename _Functor, typename _Return, typename ..._Args>
struct lambda_traits<_Return(_Functor::*)(_Args...) const>
  : __hidden__::lambda_traits_impl<_Functor, _Return, std::index_sequence_for<_Args...>, _Args...>
{
  using lambda_type = _Functor;
  using func_traits = function_traits<_Return(_Args...)>;

  using return_type = _Return;
  using args_types = std::tuple<_Args...>;
  static constexpr size_t args_size = sizeof... (_Args);
template <std::size_t _n>
  using arg_nth_type = typename std::tuple_element<_n, args_types>::type;
};
template <typename _Functor>
static inline typename lambda_traits<_Functor>::function_ref wrap_lambda_to_callback(_Functor lambda) {
  return lambda_traits<_Functor>::wrap_to_callback(lambda);
}
}
