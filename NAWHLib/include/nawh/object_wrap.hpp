#pragma once

#include <nan.h>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

#include <iostream>

//#include <nawh/method_wrap.hpp>
#include <nawh/errors.hpp>
#include <nawh/types.hpp>
#include <nawh/type_traits.hpp>
#include <nawh/constructor_traits.hpp>

namespace nawh {

template <class, bool> struct object_wrap_helper;
class object_wrap : public Nan::ObjectWrap {
  template <class, bool> friend struct object_wrap_helper;
};

namespace __hidden__ {
template <typename ..._Bases>
  struct extends_helper {
    static void __inherit(const v8::Local<v8::FunctionTemplate> &tpl, std::initializer_list<v8::Local<v8::FunctionTemplate>> parents) {
      for (auto &parent : parents) {
        tpl->Inherit(parent);
      }
    }

    static void inherit(const v8::Local<v8::FunctionTemplate> &tpl) {
      __inherit(tpl, { Nan::New<v8::FunctionTemplate>(object_wrap_helper<_Bases, true>::instance.class_template)... });
    }
  };
}

template <class _Wrapper, bool = true/*std::is_base_of<nawh::object_wrap, _Wrapper>::value*/> struct object_wrap_helper;
template <class _Wrapper> struct object_wrap_helper<_Wrapper, false>;
template <class _Wrapper>
struct object_wrap_helper<_Wrapper, true> {
private:
  object_wrap_helper() {
    // FIXME: add to every class
//    if (std::is_default_constructible<_Wrapper>::value) {
//      constructor();
//    }
  }
  static object_wrap_helper instance;
template <typename ...>
  friend struct __hidden__::extends_helper;
  object_wrap_helper(object_wrap_helper &) =delete;
  object_wrap_helper(object_wrap_helper &&) =delete;
  void operator =(object_wrap_helper &) =delete;
  void operator =(object_wrap_helper &&) =delete;

  static NAN_METHOD(construct) NAWH_TRY {
    if (!info.IsConstructCall()) {
      throw nawh::error_syntax("Must be used only as constructor.");
    }
    if (instance.constrs.size() < 1) {
      throw nawh::error_syntax("No constructors was registred for this type.");
    }
    object_wrap *obj = nullptr;
#ifdef NAWH_OPTION_ARRAY_EXACT_SIZE
    { int size = info.Length();
#else
    for (int size = info.Length(); size >= 0; --size) {
#endif
      auto range = instance.constrs.equal_range(static_cast<size_t>(size));
      for (auto it = range.first; it != range.second; ++it) {
        try {
          obj = it->second(info);
          break;
        } catch (const error_argument_type &) { }
      }
    }
    if (obj == nullptr) {
      std::cout << "error_argument_array\n";
      throw nawh::error_argument_array();
    }
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } NAWH_CATCH

  static NAN_METHOD(construct_dummy) {
    object_wrap *obj = new object_wrap();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }

public:
  static v8::Local<v8::Object> handle(_Wrapper *wrap) {
    Nan::EscapableHandleScope scope;
    if (!wrap->persistent().IsEmpty()) {
      return scope.Escape(wrap->handle());
    }
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(instance.class_template_dummy);
    auto handle = Nan::NewInstance(tpl->GetFunction()).ToLocalChecked();
    Nan::ObjectWrap::Unwrap<object_wrap>(handle)->persistent().Reset();
    Nan::SetInternalFieldPointer(handle, 0, wrap);
    return scope.Escape(handle);
  }

  using class_template_init_cb = std::function<void(object_wrap_helper *wrap)>;
template <typename ..._Bases>
  static void export_wrap(
      const std::string &name
      , Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target
      , const class_template_init_cb &class_template_init)
  {
    if (!instance.class_template.IsEmpty()) {
      throw nawh::error_syntax("You can not export a class more than once");
    }
    auto tpl = Nan::New<v8::FunctionTemplate>(&object_wrap_helper::construct);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New(name).ToLocalChecked());
    __hidden__::extends_helper<_Bases...>::inherit(tpl);
    instance.tpl = &tpl;
      class_template_init(&instance);
    instance.tpl = nullptr;
    instance.class_template.Reset(tpl);
    target->Set(Nan::New(name).ToLocalChecked(), tpl->GetFunction());

    auto tpl_dummy = Nan::New<v8::FunctionTemplate>(&object_wrap_helper::construct_dummy);
    tpl_dummy->InstanceTemplate()->SetInternalFieldCount(1);
    tpl_dummy->Inherit(tpl);
    instance.class_template_dummy.Reset(tpl_dummy);
  }
template <typename ..._Bases>
  static typename std::enable_if<
    nawh::has_class_template<_Wrapper>::value, void
  >::type export_wrap(
      const std::string &name
      , Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target)
  {
    export_wrap(name, target, &_Wrapper::class_template);
  }

template <typename ..._Bases>
  static void export_wrap_non_instantiated(
      const std::string &name
      , const class_template_init_cb &class_template_init)
  {
    if (!instance.class_template.IsEmpty()) {
      throw nawh::error_syntax("You can not export a class more than once");
    }
    auto tpl = Nan::New<v8::FunctionTemplate>(&object_wrap_helper::construct_dummy);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New(name).ToLocalChecked());
    __hidden__::extends_helper<_Bases...>::inherit(tpl);
    instance.tpl = &tpl;
      class_template_init(&instance);
    instance.tpl = nullptr;
    instance.class_template.Reset(tpl);

    auto tpl_dummy = Nan::New<v8::FunctionTemplate>(&object_wrap_helper::construct_dummy);
    tpl_dummy->InstanceTemplate()->SetInternalFieldCount(1);
    tpl_dummy->Inherit(tpl);
    instance.class_template_dummy.Reset(tpl_dummy);
  }

protected:
  v8::Local<v8::FunctionTemplate> *tpl = nullptr;
  std::unordered_multimap<size_t, NAWH_OBJECT_WRAP_CONSTRUCTOR_TYPE<_Wrapper> *> constrs;
  Nan::Persistent<v8::FunctionTemplate> class_template;
  Nan::Persistent<v8::FunctionTemplate> class_template_dummy;

public:
template <typename ..._Args>
  object_wrap_helper *constructor() {
    static_assert(std::is_constructible<_Wrapper, _Args...>::value, "Can't construct class with provided arguments");
    size_t size = sizeof... (_Args);
    auto ctor = nawh::constructor_traits<_Wrapper(_Args...)>::invoker::construct;
    constrs.emplace(size, ctor);
    return this;
  }

  template <typename _Type, typename _Cast = _Type>
  object_wrap_helper *property() {

    return this;
  }
};
template <class _Wrapper>
object_wrap_helper<_Wrapper, true> object_wrap_helper<_Wrapper, true>::instance;

template <class _Wrapper>
v8::Local<v8::Object> handle(_Wrapper *src) {
  return object_wrap_helper<_Wrapper>::handle(src);
}
}
