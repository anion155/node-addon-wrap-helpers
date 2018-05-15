#pragma once

#include <nan.h>
#include <utility>
#include <nawh/type_traits.hpp>
#include <nawh/errors.hpp>
#include <nawh/defines.hpp>
#include <nawh/converters.hpp>

namespace nawh {

namespace __hidden__ {
template <class _Wrapper, typename _Return, typename _sequence, typename ..._Args>
  struct method_wrapper;
template<class _Wrapper, typename _Return, std::size_t ..._i, typename ..._Args>
  struct method_wrapper<_Wrapper, _Return, std::integer_sequence<std::size_t, _i...>, _Args...> {
    template <_Return(_Wrapper::*_method)(_Args...)>
    static NAN_METHOD(wrapped) NAWH_TRY {
      if (info.Length() NAWH_ARRAY_INCOMPATIBLE_SIZE_OP sizeof...(_Args)) {
        throw nawh::error_argument_array(sizeof...(_Args));
      }
      _Wrapper *holder;
      try {
        holder = nawh::converter<_Wrapper *>::to_type(info.Holder());
      } catch (const nawh::error_argument_type &error) {
        throw nawh::error_reference("Bad `this` type: " + std::string(error.what()));
      }
      STATIC_IF((std::is_same<_Return, void>::value)) {
        (holder->*_method)(nawh::converter<_Args>::to_type(info[_i])...);
        info.GetReturnValue().SetUndefined();
      } STATIC_ELSE {
        auto result = (holder->*_method)(nawh::converter<_Args>::to_type(info[_i])...);
        info.GetReturnValue().Set(nawh::converter<_Return>::to_value(result));
      } STATIC_END_IF
    } NAWH_CATCH
  };
}

template <typename _Type> struct is_method : std::false_type { };
template <typename _Wrapper, typename _Return, typename ..._Args>
  struct is_method<_Return(_Wrapper::*)(_Args...)> : std::true_type { };

template <typename _Class, typename _Type> struct is_method_of : std::false_type { };
template <typename _Class, typename _Return, typename ..._Args>
  struct is_method_of<_Class, _Return(_Class::*)(_Args...)> : std::true_type { };

template <typename _Class, typename _Type>
  struct is_function_or_method_of : nawh::__or_<nawh::is_function<_Type>, nawh::is_method_of<_Class, _Type>> { };

template <typename _Type> struct method_traits;
template <class _Wrapper, typename _Return, typename ..._Args>
struct method_traits<_Return(_Wrapper::*)(_Args...)> {
  using class_type = _Wrapper;

  using return_type = _Return;
  using args_types = std::tuple<_Args...>;
  static constexpr size_t args_size = sizeof... (_Args);
template <std::size_t _n>
  using arg_nth_type = typename std::tuple_element<_n, args_types>::type;

  using function_type = _Return(_Args...);
  using function_ref = _Return(_Wrapper::*)(_Args...);
  using functor_type = std::function<function_type>;

  using wrapper = __hidden__::method_wrapper<_Wrapper, _Return, std::index_sequence_for<_Args...>, _Args...>;
};
}
