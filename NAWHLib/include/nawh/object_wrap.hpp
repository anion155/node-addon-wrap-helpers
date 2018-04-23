#pragma once

#include <nan.h>
#include <functional>
#include <string>
#include <unordered_map>

#include <nawh/errors.hpp>
#include <nawh/type_traits.hpp>
#include <nawh/utilities.hpp>

#include <nawh/constructor_traits.hpp>
#include <nawh/callback_traits.hpp>
#include <nawh/function_traits.hpp>
#include <nawh/lambda_traits.hpp>
#include <nawh/member_traits.hpp>

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
  typename std::enable_if<
    std::is_constructible<_Wrapper, _Args...>::value &&
    nawh::__and_<nawh::has_converter<_Args>...>::value
    , object_wrap_helper *
  >::type constructor() {
    size_t size = sizeof... (_Args);
    auto ctor = nawh::constructor_traits<_Wrapper(_Args...)>::wrapper::wrapped;
    constrs.emplace(size, ctor);
    return this;
  }
private:
  object_wrap_helper() {
//    if (std::is_default_constructible<_Wrapper>::value) {
//      constructor();
//    }
//    if (std::is_copy_constructible<_Wrapper>::value) {
//      constructor<const _Wrapper &>();
//    }
  }

public:
template <typename _Type, _Type _method>
  typename std::enable_if<
    std::is_member_function_pointer<_Type>::value &&
    std::is_same<typename nawh::method_traits<_Type>::class_type, _Wrapper>::value
    , object_wrap_helper *
  >::type method(const std::string &name) {
    auto cb = &nawh::method_traits<_Type>::wrapper::template wrapped<_method>;
    Nan::SetPrototypeMethod(*tpl, name.c_str(), cb);
    return this;
  }
template <typename _Type, _Type _function>
  typename std::enable_if<
    std::is_function<typename std::remove_pointer<_Type>::type>::value
    , object_wrap_helper *
  >::type method(const std::string &name) {
    auto cb = &nawh::function_traits<_Type>::wrapper::template wrapped<_function>;
    Nan::SetPrototypeMethod(*tpl, name.c_str(), cb);
    return this;
  }
#ifdef __cpp_template_auto
template <auto _method>
  object_wrap_helper *method(const std::string &name) {
    return method<decltype (_method), _method>(name);
  }
#endif

public:
template <typename _Functor>
  typename std::enable_if<
    nawh::is_lambda<_Functor>::value
    , object_wrap_helper *
  >::type method_lambda(_Functor lambda, const std::string &name) {
    Nan::SetPrototypeMethod(*tpl, name.c_str(), nawh::lambda_traits<_Functor>::wrap_lambda_to_nan(lambda));
    return this;
  }

template <typename _Type, _Type _member, typename _PropertyType = typename nawh::member_traits<_Type>::member_type>
  typename std::enable_if<
    std::is_member_object_pointer<_Type>::value &&
    std::is_same<typename nawh::member_traits<_Type>::class_type, _Wrapper>::value
    , object_wrap_helper *
  >::type accessor(const std::string &name) {
    auto getter = nawh::member_traits<_Type>::template wrapper<_PropertyType>::template getter<_member>;
    auto setter = nawh::member_traits<_Type>::template wrapper<_PropertyType>::template setter<_member>;
    Nan::SetAccessor((*tpl)->InstanceTemplate(), Nan::New(name).ToLocalChecked(), getter, setter);
    return this;
  }
#ifdef __cpp_template_auto
template <auto _member, typename _PropertyType = typename nawh::member_traits<decltype (_member)>::member_type>
  object_wrap_helper *accessor(const std::string &name) {
    return accessor<decltype (_member), _member, _PropertyType>(name);
  }
#endif

template <typename _GetterType, _GetterType _getter, typename _SetterType, _SetterType _setter>
  object_wrap_helper *accessor(const std::string &name) {
    auto getter = nawh::callback_traits<_GetterType>::wrapper::template wrapped<_getter>;
    auto setter = nawh::callback_traits<_SetterType>::wrapper::template wrapped<_setter>;
    Nan::SetAccessor((*tpl)->InstanceTemplate(), Nan::New(name).ToLocalChecked(), getter, setter);
    return this;
  }
};

template <class _Wrapper>
v8::Local<v8::Object> handle(_Wrapper *src) {
  return object_wrap_helper<_Wrapper>::handle(src);
}
}
