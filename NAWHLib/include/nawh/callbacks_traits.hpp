#pragma once

#include <nawh/function_traits.hpp>
#include <nawh/method_traits.hpp>
#include <nawh/lambda_traits.hpp>

namespace nawh {
template <typename _Type> struct callback_traits;
template <typename _Return, typename ..._Args>
  struct callback_traits<_Return(_Args...)> : nawh::function_traits<_Return(*)(_Args...)> { };
  template <typename _Return, typename ..._Args>
    struct callback_traits<_Return(*)(_Args...)> : nawh::function_traits<_Return(*)(_Args...)> { };
template <typename _Wrapper, typename _Return, typename ..._Args>
  struct callback_traits<_Return(_Wrapper::*)(_Args...)> : nawh::method_traits<_Return(_Wrapper::*)(_Args...)> { };

template <typename _Type> struct is_callback
      : nawh::__or_<nawh::is_function<_Type>, nawh::is_method<_Type>>
  { };
}
