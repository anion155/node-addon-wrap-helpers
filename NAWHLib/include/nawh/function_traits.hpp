#pragma once

#include <nan.h>
#include <nawh/defines.hpp>
#include <nawh/errors.hpp>
#include <nawh/converters.hpp>

namespace nawh {

namespace __hidden__ {
template <typename _Return, typename _sequence, typename ..._Args>
  struct function_wrapper;
template<typename _Return, std::size_t ..._i, typename ..._Args>
  struct function_wrapper<_Return, std::integer_sequence<std::size_t, _i...>, _Args...> {
    template <_Return(*_function)(_Args...)>
    static NAN_METHOD(wrapped) NAWH_TRY {
      if (info.Length() NAWH_ARRAY_INCOMPATIBLE_SIZE_OP sizeof...(_Args)) {
        throw nawh::error_argument_array(sizeof...(_Args));
      }
      if constexpr (std::is_same<_Return, void>::value) {
        _function(nawh::converter<_Args>::to_type(info[_i])...);
        info.GetReturnValue().SetUndefined();
      } else {
        auto result = _function(nawh::converter<_Args>::to_type(info[_i])...);
        info.GetReturnValue().Set(nawh::converter<_Return>::to_value(result));
      }
    } NAWH_CATCH
  };
}

template <typename _Type> struct is_function : std::is_function<typename std::remove_pointer<_Type>::type> { };

template <typename _Type> struct function_traits;
template <typename _Return, typename ..._Args>
struct function_traits<_Return(_Args...)> {
  using return_type = _Return;
  using args_types = std::tuple<_Args...>;
  static constexpr size_t args_size = sizeof... (_Args);
template <std::size_t _n>
  using arg_nth_type = typename std::tuple_element<_n, args_types>::type;

  using function_type = _Return(_Args...);
  using function_ref = function_type *;
  using functor_type = std::function<function_type>;

  using wrapper = __hidden__::function_wrapper<_Return, std::index_sequence_for<_Args...>, _Args...>;
};
template <typename _Return, typename ..._Args>
struct function_traits<_Return(*)(_Args...)> : public function_traits<_Return(_Args...)> { };
}
