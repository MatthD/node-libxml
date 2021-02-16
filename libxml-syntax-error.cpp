/**
* This is the Errors Class script, for wellformed & validation DTD
*/
#include <cstring>
#include <iostream>
#include "libxml-syntax-error.h"

void setStringField(Napi::Object obj, const char *name, const char *value, Napi::Env env)
{
  Napi::HandleScope scope(env);
  if (!value)
  {
    return;
  }
  obj.Set(name, Napi::String::New(env, value));
}

void setNumericField(Napi::Object obj, const char *name, const int value, Napi::Env env)
{
  Napi::HandleScope scope(env);
  obj.Set(name, Napi::Number::New(env, value));
}

Napi::Value
XmlSyntaxError::BuildSyntaxError(xmlError *error, Napi::Env env)
{
  Napi::EscapableHandleScope scope(env);

  auto err = Napi::TypeError::New(env,
                                  Napi::String::New(env, error->message));
  Napi::Object out = Napi::Object::New(env);

  setStringField(out, "message", error->message, env);
  setNumericField(out, "level", error->level, env);
  setNumericField(out, "column", error->int2, env);
  setStringField(out, "file", error->file, env);
  setNumericField(out, "line", error->line, env);

  if (error->int1)
  {
    setNumericField(out, "int1", error->int1, env);
  }
  return out;
}

uint32_t XmlSyntaxError::maxError{100};
Napi::Env* XmlSyntaxError::env = nullptr;

void XmlSyntaxError::ChangeMaxNumberOfError(int max)
{
  XmlSyntaxError::maxError = max;
}

uint32_t XmlSyntaxError::GetMaxNumberOfError() {
  return XmlSyntaxError::maxError;
}

void XmlSyntaxError::PushToArray(void *errs, xmlError *error)
{
  Napi::Array errors = *reinterpret_cast<Napi::Array *>(errs);
  if (errors.Length() >= maxError)
  {
    return;
  }
  Napi::Value castedError = {XmlSyntaxError::BuildSyntaxError(error, *XmlSyntaxError::env)};
  errors.Set(errors.Length(), castedError);
}

void XmlSyntaxError::PushToArray(Napi::Array& errors, const char* errorMessage)
{
  if (errors.Length() >= maxError)
  {
    return;
  }
  Napi::String messageToPush = Napi::String::New(*XmlSyntaxError::env, errorMessage);
  errors.Set(errors.Length(), messageToPush);
}

MaxErrorNumberRestorer::MaxErrorNumberRestorer(): max(XmlSyntaxError::GetMaxNumberOfError()) { }

MaxErrorNumberRestorer::~MaxErrorNumberRestorer() {
  XmlSyntaxError::ChangeMaxNumberOfError(max);
}