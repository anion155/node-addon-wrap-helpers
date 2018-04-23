#pragma once

#include <nawh/function_traits.hpp>
#include <nawh/method_traits.hpp>
#include <nawh/lambda_traits.hpp>

namespace nawh {

template <typename _Functor>
  struct callback_traits : public std::enable_if<nawh::is_lambda<_Functor>::value, nawh::lambda_traits<_Functor>>::type { };
template <typename _Return, typename ..._Args>
  struct callback_traits<_Return(*)(_Args...)> : public function_traits<_Return(_Args...)> { };
template <typename _Class, typename _Return, typename ..._Args>
  struct callback_traits<_Return(_Class::*)(_Args...)> : public method_traits<_Return(_Class::*)(_Args...)> { };
}
