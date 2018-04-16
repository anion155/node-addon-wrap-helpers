#pragma once

#include <exception>
#include <string>
#include <nan.h>

namespace nawh {

class error : public std::exception {
  std::string m_msg;

public:
  error(const std::string &msg);
  error(const v8::Local<v8::String> &msg);
  error(const v8::Local<v8::Message> &msg);
  error(const v8::Local<v8::Value> &error);

  virtual v8::Local<v8::Value> js_error() const;
  const char *what() const noexcept override;
};

class error_range : public nawh::error {
public:
  error_range(const std::string &msg);
  v8::Local<v8::Value> js_error() const override;
};

class error_reference : public nawh::error {
public:
  error_reference(const std::string &msg);
  v8::Local<v8::Value> js_error() const override;
};

class error_syntax : public nawh::error {
public:
  error_syntax(const std::string &msg);
  v8::Local<v8::Value> js_error() const override;
};

class error_type : public nawh::error {
public:
  error_type(const std::string &msg);
  v8::Local<v8::Value> js_error() const override;
};

class error_argument_type : public nawh::error_type {
public:
  error_argument_type();
  error_argument_type(const std::string &msg);
};

class error_argument_empty : public nawh::error_argument_type {
public:
  error_argument_empty();
};

class error_argument_array : public nawh::error_argument_type {
public:
  error_argument_array();
  error_argument_array(const std::size_t &expected);
};
}

#ifdef NAWH_OPTION_EXCEPTIONS_TO_JS
#  define NAWH_TRY \
    { try
#  define NAWH_CATCH_RET(error_ret) \
    catch (const nawh::error &e) { \
      Nan::ThrowError(e.js_error()); \
      return (error_ret); \
    } catch (const std::exception &e) { \
      Nan::ThrowError(e.what()); \
      return (error_ret); \
    } \
  }
#  define NAWH_CATCH \
    NAWH_CATCH_RET((void)0)
#else
#  define NAWH_TRY
#  define NAWH_CATCH_RET(error_ret)
#  define NAWH_CATCH
#endif
