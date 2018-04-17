  #pragma once

#include <string>
#include <type_traits>
#include <memory>
#include <functional>
#include <vector>
#include <nan.h>

#include <nawh/object_wrap.hpp>
#include <nawh/type_traits.hpp>
#include <nawh/errors.hpp>

namespace nawh {

class object_wrap;
template <typename, typename> struct converter;

namespace __hidden__ {
template <typename _Type>
  struct is_cstring_type
      : std::integral_constant<bool,
      std::is_same<typename std::decay<typename std::remove_pointer<_Type>::type>::type *, char *>::value
      || std::is_same<typename std::decay<typename std::remove_pointer<_Type>::type>::type *, wchar_t *>::value> { };

template <typename _Type>
  struct is_string_object_type
      : std::integral_constant<bool,
      std::is_same<typename std::decay<_Type>::type, std::string>::value
      || std::is_same<typename std::decay<_Type>::type, std::wstring>::value
      || std::is_same<typename std::decay<_Type>::type, std::u16string>::value
      || std::is_same<typename std::decay<_Type>::type, std::u32string>::value> { };

template <typename _Type>
  struct converter_base {
    using type = _Type;

    static void check(const v8::Local<v8::Value> &value) {
      if (value.IsEmpty() || value->IsNullOrUndefined()) {
        throw nawh::error_argument_empty();
      }
      if (!converter<_Type, void>::is_type(value)) {
        throw nawh::error_argument_type();
      }
    }

//    static bool is_type(const v8::Local<v8::Value> &value) { return value->Is(); }
//    static _Type to_type(const v8::Local<v8::Value> &value) { }
//    static v8::Local<v8::Value> to_value(const _Type &value) { }
  };

  struct converter_bool : converter_base<bool> {
    static bool is_type(const v8::Local<v8::Value> &value) { return value->IsBoolean() || value->IsBooleanObject(); }
    static type to_type(const v8::Local<v8::Value> &value) {
      converter_bool::check(value);
      auto maybe = value->BooleanValue(Nan::GetCurrentContext());
      if (!maybe.IsJust()) { throw nawh::error_argument_type(); }
      return maybe.ToChecked();
    }
    static v8::Local<v8::Value> to_value(const type &value) { Nan::EscapableHandleScope scope; return scope.Escape(Nan::New(value)); }
  };

template <typename _Type>
  struct converter_integer : converter_base<_Type> {
    static bool is_type(const v8::Local<v8::Value> &value) { return value->IsInt32(); }
    static _Type to_type(const v8::Local<v8::Value> &value) {
      converter_integer::check(value);
      auto maybe = value->ToInteger(Nan::GetCurrentContext());
      if (maybe.IsEmpty()) { throw nawh::error_argument_type(); }
      return static_cast<_Type>(maybe.ToLocalChecked()->Value());
    }
    static v8::Local<v8::Value> to_value(const _Type &value) { Nan::EscapableHandleScope scope; return scope.Escape(Nan::New(value)); }
  };

template <typename _Type>
  struct converter_real : converter_base<_Type> {
    static bool is_type(const v8::Local<v8::Value> &value) { return value->IsNumber() || value->IsNumberObject(); }
    static _Type to_type(const v8::Local<v8::Value> &value) {
      converter_real::check(value);
      auto maybe = value->NumberValue(Nan::GetCurrentContext());
      if (!maybe.IsJust()) { throw nawh::error_argument_type(); }
      return static_cast<_Type>(maybe.ToChecked());
    }
    static v8::Local<v8::Value> to_value(const _Type &value) { Nan::EscapableHandleScope scope; return scope.Escape(Nan::New(value)); }
  };

template <typename _Type>
  struct converter_cstring : converter_base<_Type *> {
    static bool is_type(const v8::Local<v8::Value> &value) { return value->IsString() || value->IsStringObject(); }
    [[deprecated("Pass ownership of created object to caller. Has to freed manualy. Use std::string instead")]]
    static _Type *to_type(const v8::Local<v8::Value> &value) {
      converter_cstring::check(value);
      Nan::Utf8String utf8(value);
      _Type *buf = new _Type[strlen(*utf8)];
      strcpy(buf, *utf8);
      return buf;
    }
    static v8::Local<v8::Value> to_value(const _Type *value) { Nan::EscapableHandleScope scope; return scope.Escape(Nan::New(value).ToLocalChecked()); }
  };

template <typename _Type>
  struct converter_string : converter_base<_Type> {
    static bool is_type(const v8::Local<v8::Value> &value) { return value->IsString() || value->IsStringObject(); }
    static _Type to_type(const v8::Local<v8::Value> &value) {
      converter_string::check(value);
      Nan::Utf8String utf8(value);
      return _Type(*utf8);
    }
    static v8::Local<v8::Value> to_value(const _Type &value) { Nan::EscapableHandleScope scope; return scope.Escape(Nan::New(value).ToLocalChecked()); }
  };

template <typename _Type>
  struct converter_wrapper : converter_base<_Type *> {
    static bool is_type(const v8::Local<v8::Value> &value) { return value->IsObject(); }
    static _Type *to_type(const v8::Local<v8::Value> &value) {
      converter_wrapper::check(value);
      auto wrap = Nan::ObjectWrap::Unwrap<nawh::object_wrap>(value->ToObject());
      if (!wrap) { throw nawh::error_argument_type("Value is not cpp object"); }
      auto obj = dynamic_cast<_Type *>(wrap);
      if (!obj) { throw nawh::error_argument_type("Object is not wanted type"); }
      return obj;
    }
    static v8::Local<v8::Value> to_value(_Type *value) {
      Nan::EscapableHandleScope scope;
      return scope.Escape(object_wrap_helper<_Type, true>::handle(value));
    }
  };

template <typename _SmartPtr, typename _Type>
  struct converter_wrapper_smart : converter_base<_Type *> {
    static bool is_type(const v8::Local<v8::Value> &value) { return value->IsObject(); }
    static _SmartPtr to_type(const v8::Local<v8::Value> &value) {
      return _SmartPtr(converter_wrapper<_Type>::to_type(value));
    }
    static v8::Local<v8::Value> to_value(const _SmartPtr &value) {
      Nan::EscapableHandleScope scope;
      return scope.Escape(converter_wrapper<_Type>::to_value(value.operator ->()));
    }
  };

//template <typename _Type>
//  struct converter_function : converter_base<_Type> { };

template <typename _Type>
  struct converter_v8 : converter_base<v8::Local<_Type>> {
    static bool is_type(const v8::Local<v8::Value> &) { return true; }
    static v8::Local<_Type> to_type(const v8::Local<v8::Value> &value) {
      converter_v8::check(value);
      return Nan::To<_Type>(value).ToLocalChecked();
    }
    static v8::Local<v8::Value> to_value(v8::Local<_Type> value) { Nan::EscapableHandleScope scope; return scope.Escape(value); }
  };

template <typename _Type, std::size_t _size>
  struct converter_raw_array : converter_base<_Type[_size]> {
    static bool is_type(const v8::Local<v8::Value> &value) { return value->IsArray(); }
    static _Type *to_type(const v8::Local<v8::Value> &value) {
      converter_raw_array::check(value);
      auto array = value.As<v8::Array>();
      if (array->Length() NAWH_ARRAY_INCOMPATIBLE_SIZE_OP _size) {
        throw nawh::error_argument_array(_size);
      }
      _Type *buf = new _Type[_size];
      for (size_t i = 0; i < _size; ++i) {
        buf[i] = nawh::converter<_Type, void>::to_type(array->Get(i));
      }
      return buf;
    }
    static v8::Local<v8::Value> to_value(_Type *value) {
      Nan::EscapableHandleScope scope;
      auto array = Nan::New<v8::Array>(_size);
      for (size_t i = 0; i < _size; ++i) {
        array->Set(i, nawh::converter<_Type, void>::to_value(value[i]));
      }
      return scope.Escape(array);
    }
  };

template <typename _Type, std::size_t _size>
  struct converter_array : converter_base<std::array<_Type, _size>> {
    static bool is_type(const v8::Local<v8::Value> &value) { return value->IsArray(); }
    static std::array<_Type, _size> &&to_type(const v8::Local<v8::Value> &value) {
      converter_array::check(value);
      auto array = value.As<v8::Array>();
      if (array->Length() NAWH_ARRAY_INCOMPATIBLE_SIZE_OP _size) {
        throw nawh::error_argument_array(_size);
      }
      std::array<_Type, _size> buf;
      for (size_t i = 0; i < _size; ++i) {
        buf[i] = nawh::converter<_Type, void>::to_type(array->Get(i));
      }
      return std::move(buf);
    }
    static v8::Local<v8::Value> to_value(const std::array<_Type, _size> &value) {
      Nan::EscapableHandleScope scope;
      auto array = Nan::New<v8::Array>(_size);
      for (size_t i = 0; i < _size; ++i) {
        array->Set(i, nawh::converter<_Type, void>::to_value(value[i]));
      }
      return scope.Escape(array);
    }
  };

template <typename _Type>
  struct converter_vector : converter_base<std::vector<_Type>> {
    static bool is_type(const v8::Local<v8::Value> &value) { return value->IsArray(); }
    static std::vector<_Type> &&to_type(const v8::Local<v8::Value> &value) {
      converter_vector::check(value);
      auto array = value.As<v8::Array>();
      std::vector<_Type> buf; buf.reserve(array->Length());
      for (size_t i = 0, s = array->Length(); i < s; ++i) {
        buf.push_back(nawh::converter<_Type, void>::to_type(array->Get(i)));
      }
      return std::move(buf);
    }
    static v8::Local<v8::Value> to_value(const std::vector<_Type> &value) {
      Nan::EscapableHandleScope scope;
      auto array = Nan::New<v8::Array>(value.size());
      for (size_t i = 0; i < value.size(); ++i) {
        array->Set(i, nawh::converter<_Type, void>::to_value(value[i]));
      }
      return scope.Escape(array);
    }
  };
}

template <typename _Type, typename = void> struct converter;
template <typename _Type>
struct converter<_Type, typename std::enable_if<std::is_same<std::remove_reference_t<_Type>, bool>::value>::type>
    : __hidden__::converter_bool
{ };
template <typename _Type>
struct converter<_Type, typename std::enable_if<std::is_integral<std::remove_reference_t<_Type>>::value && !std::is_same<_Type, bool>::value>::type>
    : __hidden__::converter_integer<std::remove_cv_t<_Type>>
{ };
//template <typename _Type>
//struct converter<_Type, typename std::enable_if<std::is_enum<std::remove_reference_t<_Type>>::value>::type>
//    : __hidden__::converter_arithmetic<std::remove_cv_t<_Type>>
//{ };
template <typename _Type>
struct converter<_Type, typename std::enable_if<std::is_floating_point<std::remove_reference_t<_Type>>::value>::type>
    : __hidden__::converter_real<std::remove_cv_t<_Type>>
{ };
template <typename _Type>
struct converter<_Type *, typename std::enable_if<__hidden__::is_cstring_type<_Type *>::value>::type>
    : __hidden__::converter_cstring<std::remove_cv_t<_Type>>
{ };
template <typename _Type>
struct converter<_Type, typename std::enable_if<__hidden__::is_string_object_type<_Type>::value>::type>
    : __hidden__::converter_string<std::remove_cv_t<_Type>>
{ };
template <typename _Type>
struct converter<_Type *, typename std::enable_if<std::is_base_of<nawh::object_wrap, _Type>::value>::type>
    : __hidden__::converter_wrapper<std::remove_cv_t<_Type>>
{ };
template <typename _Type>
struct converter<std::shared_ptr<_Type>, typename std::enable_if<std::is_base_of<nawh::object_wrap, _Type>::value>::type>
    : __hidden__::converter_wrapper_smart<std::shared_ptr<std::remove_cv_t<_Type>>, std::remove_cv_t<_Type>>
{ };
//template <typename _Rt, typename ..._As>
//struct converter<std::function<_Rt(_As...)>, typename std::true_type::type>
//  : __hidden__::converter_function<std::function<_Rt(_As...)>>
//{ };
template <typename _Type>
struct converter<v8::Local<_Type>, typename std::enable_if<std::is_base_of<v8::Value, _Type>::value>::type>
    : __hidden__::converter_v8<v8::Local<_Type>>
{ };
template <typename _Type, std::size_t _size>
struct converter<_Type[_size], typename std::enable_if<
    nawh::has_converter<_Type>::value
    && !nawh::__hidden__::is_cstring_type<_Type[_size]>::value>::type>
    : __hidden__::converter_raw_array<std::remove_cv_t<_Type>, _size>
{ };
template <typename _Type, std::size_t _size>
struct converter<std::array<_Type, _size>, typename std::enable_if<nawh::has_converter<_Type>::value>::type>
    : __hidden__::converter_array<std::remove_cv_t<_Type>, _size>
{ };
template <typename _Type>
struct converter<std::vector<_Type>, typename std::enable_if<nawh::has_converter<_Type>::value>::type>
    : __hidden__::converter_vector<std::remove_cv_t<_Type>>
{ };
}
