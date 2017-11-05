/**
* This is the Errros Class script, for wellformed & validation DTD
*/
#include <cstring>
#include "libxml-syntax-error.h"

void setStringField(v8::Local<v8::Object> obj, const char* name, const char* value) {
  Nan::HandleScope scope;
  if (!value) {
    return;
  }
  Nan::Set(obj, Nan::New<v8::String>(name).ToLocalChecked(), Nan::New<v8::String>(value, strlen(value)).ToLocalChecked());
}

void setNumericField(v8::Local<v8::Object> obj, const char* name, const int value) {
  Nan::HandleScope scope;
  Nan::Set(obj, Nan::New<v8::String>(name).ToLocalChecked(), Nan::New<v8::Int32>(value));
}

v8::Local<v8::Value>
XmlSyntaxError::BuildSyntaxError(xmlError* error) {
  Nan::EscapableHandleScope scope;

  v8::Local<v8::Value> err = v8::Exception::Error(
    Nan::New<v8::String>(error->message).ToLocalChecked());
  v8::Local<v8::Object> out = v8::Local<v8::Object>::Cast(err);

  setStringField(out, "message", error->message);
  setNumericField(out, "level", error->level);
  setNumericField(out, "column", error->int2);
  setStringField(out, "file", error->file);
  setNumericField(out, "line", error->line);

  if (error->int1) {
    setNumericField(out, "int1", error->int1);
  }
  return scope.Escape(err);
}

void
XmlSyntaxError::PushToArray(void* errs, xmlError* error) {
  Nan::HandleScope scope;
  v8::Local<v8::Array> errors = *reinterpret_cast<v8::Local<v8::Array>*>(errs);
  v8::Local<v8::Function> push = v8::Local<v8::Function>::Cast(errors->Get(Nan::New<v8::String>("push").ToLocalChecked()));

  v8::Local<v8::Value> argv[1] = { XmlSyntaxError::BuildSyntaxError(error) };
  push->Call(errors, 1, argv);
}