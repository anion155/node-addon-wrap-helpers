#pragma once

#include <nan.h>
#include <functional>
#include <string>
#include <unordered_map>

#include <nawh/errors.hpp>
#include <nawh/type_traits.hpp>
#include <nawh/utilities.hpp>

#include <nawh/constructor_traits.hpp>
#include <nawh/callbacks_traits.hpp>
#include <nawh/accessors.hpp>

namespace nawh {

template <class, typename> struct object_wrap_helper;
class object_wrap : public Nan::ObjectWrap {
  template <class, typename> friend struct object_wrap_helper;
};

template <class, typename> struct object_wrap_helper;
namespace __hidden__ {
template <typename _Base = void>
  struct extends_helper {
    static void inherit(const v8::Local<v8::FunctionTemplate> &tpl) {
      tpl->Inherit(Nan::New<v8::FunctionTemplate>(object_wrap_helper<_Base, void>::instance()->class_template));
    }
  };
template <>
  struct extends_helper<void> {
    static void inherit(const v8::Local<v8::FunctionTemplate> &tpl) {
      nawh::UNUSED(tpl);
    }
  };
}

template <class _Wrapper, typename = void> struct object_wrap_helper;
template <class _Wrapper>
struct object_wrap_helper<_Wrapper, typename std::enable_if<std::is_base_of<nawh::object_wrap, _Wrapper>::value>::type> {
private:
  static object_wrap_helper *instance() {
    static object_wrap_helper *inst = new object_wrap_helper();
    return inst;
  }

  object_wrap_helper(object_wrap_helper &) =delete;
  object_wrap_helper(object_wrap_helper &&) =delete;
  void operator =(object_wrap_helper &) =delete;
  void operator =(object_wrap_helper &&) =delete;

  template <typename> friend struct __hidden__::extends_helper;
  Nan::Persistent<v8::FunctionTemplate> class_template;

public:
  using class_template_init_cb = std::function<void(object_wrap_helper *wrap)>;
template <typename ..._Bases>
  static void export_wrap(
      const std::string &name
      , Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target
      , const class_template_init_cb &class_template_init)
  {
    if (!instance()->class_template.IsEmpty()) {
      throw nawh::error_syntax("You can not export a class more than once");
    }

    auto tpl = Nan::New<v8::FunctionTemplate>(&construct);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New(name).ToLocalChecked());
    __hidden__::extends_helper<_Bases...>::inherit(tpl);

    instance()->tpl = &tpl;
    class_template_init(instance());
    instance()->tpl = nullptr;

    instance()->class_template.Reset(tpl);
    if (!target.IsEmpty()) {
      target->Set(Nan::New(name).ToLocalChecked(), tpl->GetFunction());
    }
  }
template <typename ..._Bases>
  static typename std::enable_if<
    nawh::has_class_template<_Wrapper>::value
  >::type export_wrap(const std::string &name, Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target) {
    export_wrap<_Bases...>(name, target, &_Wrapper::class_template);
  }
template <typename ..._Bases>
  static void export_wrap_non_instantiated(const std::string &name, const class_template_init_cb &class_template_init) {
    export_wrap<_Bases...>(name, Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE(), class_template_init);
  }
template <typename ..._Bases>
  static void export_wrap_non_instantiated(const std::string &name) {
    export_wrap<_Bases...>(name, Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE(), &_Wrapper::class_template);
  }

public:
  static v8::Local<v8::Object> handle(_Wrapper *wrap) {
    Nan::EscapableHandleScope scope;
    if (!wrap->persistent().IsEmpty()) {
      return scope.Escape(wrap->handle());
    }
    v8::Local<v8::FunctionTemplate> tpl = Nan::New(instance()->class_template);
    v8::Local<v8::Object> handle = Nan::NewInstance(tpl->InstanceTemplate()).ToLocalChecked();
    wrap->Wrap(handle);
    return scope.Escape(handle);
  }

private:
  static NAN_METHOD(construct) NAWH_TRY {
    if (!info.IsConstructCall()) {
      throw nawh::error_syntax("Must be used only as constructor.");
    }
    if (instance()->constrs.size() < 1) {
      throw nawh::error_syntax("No constructors was registred for this type.");
    }
    object_wrap *obj = nullptr;
#ifdef NAWH_OPTION_ARRAY_EXACT_SIZE
    { int size = info.Length();
#else
    for (int size = info.Length(); size >= 0; --size) {
#endif
      auto range = instance()->constrs.equal_range(static_cast<size_t>(size));
      for (auto it = range.first; it != range.second; ++it) {
        try {
          obj = it->second(info);
          break;
        } catch (const error_argument_type &) {
          continue;
        }
      }
    }
    if (obj == nullptr) {
      throw nawh::error_argument_array();
    }
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } NAWH_CATCH

private:
  v8::Local<v8::FunctionTemplate> *tpl = nullptr;

private:
  std::unordered_multimap<size_t, _Wrapper *(*)(Nan::NAN_METHOD_ARGS_TYPE info)> constrs;
public:
template <typename ..._Args>
  object_wrap_helper *constructor() {
    static_assert (std::is_constructible<_Wrapper, _Args...>::value, "Wrapper is not constructible with provided arguments");
    static_assert (nawh::__and_<nawh::has_converter<_Args>...>::value, "Arguments can not be converted from js types");
    size_t size = sizeof... (_Args);
    auto ctor = nawh::constructor_traits<_Wrapper(_Args...)>::wrapper::wrapped;
    constrs.emplace(size, ctor);
    return this;
  }
  object_wrap_helper *constructor_default() {
    static_assert (std::is_default_constructible<_Wrapper>::value, "Wrapper is not default constructible");
    return constructor<>();
  }
  object_wrap_helper *constructor_copy() {
    static_assert (std::is_copy_constructible<_Wrapper>::value, "Wrapper is not copy constructible");
    return constructor<const _Wrapper &>();
  }
private:
  object_wrap_helper() {
    if constexpr (std::is_default_constructible<_Wrapper>::value) {
      this->constructor_default();
    }
    if constexpr (std::is_copy_constructible<_Wrapper>::value) {
      this->constructor_copy();
    }
  }

public:
template <auto _callback>
  object_wrap_helper *method(const std::string &name) {
    using _Type = decltype (_callback);
    static_assert (nawh::is_function_or_method_of<_Wrapper, _Type>::value, "Callback must point to function "
                                                                           "or static method "
                                                                           "or method of the current wrapper");
    auto cb = &nawh::callback_traits<_Type>::wrapper::template wrapped<_callback>;
    Nan::SetPrototypeMethod(*tpl, name.c_str(), cb);
    return this;
  }
template <typename _Functor>
  object_wrap_helper *method(_Functor lambda, const std::string &name) {
    static_assert (nawh::is_lambda<_Functor>::value, "Seems like argument is not lambda");
    Nan::SetPrototypeMethod(*tpl, name.c_str(), nawh::lambda_traits<_Functor>::wrap_lambda_to_nan(lambda));
    return this;
  }


  template <auto _getter, auto _setter, typename _ConvertType = void>
  object_wrap_helper * accessor(const std::string &name) {
    using _GetterType = decltype (_getter);
    using _SetterType = decltype (_setter);

    static_assert (nawh::__hidden__::is_accessor_getter_type<_GetterType>::value, "Getter is not suitable");
    static_assert (nawh::__hidden__::is_accessor_setter_type<_SetterType>::value, "Setter is not suitable");
    static_assert (nawh::is_accessor_types<_GetterType, _SetterType>::value, "Getter and Setter is not compatible");

    using DefaultType = typename accessor_type<_GetterType, _SetterType>::type;
    using ConvertType = typename nawh::if_else_type<!std::is_same<_ConvertType, void>::value, _ConvertType, DefaultType>::type;

    Nan::SetAccessor(
          (*tpl)->InstanceTemplate(), Nan::New(name).ToLocalChecked(),
          &nawh::accessor_getter<_GetterType, _getter, ConvertType>::getter,
          &nawh::accessor_setter<_SetterType, _setter, ConvertType>::setter
          );
    return this;
  }

template <auto _member, typename _ConvertType = void>
  object_wrap_helper * accessor(const std::string &name) {
    return accessor<_member, _member, _ConvertType>(name);
  }
};

template <class _Wrapper>
v8::Local<v8::Object> handle(_Wrapper *src) {
  return object_wrap_helper<_Wrapper>::handle(src);
}
}
