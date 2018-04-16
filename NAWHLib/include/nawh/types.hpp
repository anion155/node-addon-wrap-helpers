#pragma once

#include <nan.h>

namespace nawh {

#define NAWH_CLASS_TEMPLATE(CLASS) void class_template(const nawh::object_wrap_helper<CLASS> &wrap)

template <class _Wrapper>
using NAWH_OBJECT_WRAP_CONSTRUCTOR_TYPE = _Wrapper *(Nan::NAN_METHOD_ARGS_TYPE info);
}
