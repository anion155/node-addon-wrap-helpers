#include <nawh/errors.hpp>

nawh::error::error(const std::string &msg) : m_msg(msg) { }
nawh::error::error(const v8::Local<v8::String> &msg) { Nan::Utf8String utf8(msg); m_msg = *utf8; }
nawh::error::error(const v8::Local<v8::Message> &msg) : error(msg->Get()) { }
nawh::error::error(const v8::Local<v8::Value> &error) {
  Nan::HandleScope scope;
  auto msg = v8::Exception::CreateMessage(Nan::GetCurrentContext()->GetIsolate(), error);
  Nan::Utf8String utf8(msg->Get());
  m_msg = *utf8;
}
v8::Local<v8::Value> nawh::error::js_error() const {
  Nan::EscapableHandleScope scope;
  return scope.Escape(Nan::Error(what()));
}
const char *nawh::error::what() const noexcept {
  return m_msg.c_str();
}

nawh::error_range::error_range(const std::string &msg) : error(msg) { }
v8::Local<v8::Value> nawh::error_range::js_error() const {
  Nan::EscapableHandleScope scope;
  return scope.Escape(Nan::RangeError(what()));
}

nawh::error_reference::error_reference(const std::string &msg) : error(msg) { }
v8::Local<v8::Value> nawh::error_reference::js_error() const {
  Nan::EscapableHandleScope scope;
  return scope.Escape(Nan::ReferenceError(what()));
}

nawh::error_syntax::error_syntax(const std::string &msg) : error(msg) { }
v8::Local<v8::Value> nawh::error_syntax::js_error() const {
  Nan::EscapableHandleScope scope;
  return scope.Escape(Nan::SyntaxError(what()));
}

nawh::error_type::error_type(const std::string &msg) : error(msg) { }
v8::Local<v8::Value> nawh::error_type::js_error() const {
  Nan::EscapableHandleScope scope;
  return scope.Escape(Nan::TypeError(what()));
}

nawh::error_argument_type::error_argument_type()
  : nawh::error_type("Argument has incompatible type")
{ }
nawh::error_argument_type::error_argument_type(const std::string &msg)
  : nawh::error_type("Argument has incompatible type: "+msg)
{ }

nawh::error_argument_empty::error_argument_empty() : nawh::error_argument_type("<null>") { }

#ifdef NAWH_OPTION_ARRAY_EXACT_SIZE
nawh::error_argument_array::error_argument_array() : nawh::error_argument_type("Incorrect elements count in array") { }
nawh::error_argument_array::error_argument_array(const std::size_t &expected) : nawh::error_argument_type("Incorrect elements count in array, expected: " + std::to_string(expected)) { }
#else
nawh::error_argument_array::error_argument_array() : nawh::error_argument_type("Too few elements in array") { }
nawh::error_argument_array::error_argument_array(const std::size_t &expected) : nawh::error_argument_type("Too few elements in array, expected: " + std::to_string(expected)) { }
#endif
