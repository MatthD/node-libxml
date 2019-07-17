/**
* This is the Errros Class script, for wellformed & validation DTD
*/
#include <cstring>
#include <iostream>
#include "libxml-syntax-error.h"

using namespace v8;

void setStringField(Local<Object> obj, const char* name, const char* value) {
  Nan::HandleScope scope;
  if (!value) {
    return;
  }
  Nan::Set(obj, Nan::New<String>(name).ToLocalChecked(), Nan::New<String>(value, strlen(value)).ToLocalChecked());
}

void setNumericField(Local<Object> obj, const char* name, const int value) {
  Nan::HandleScope scope;
  Nan::Set(obj, Nan::New<String>(name).ToLocalChecked(), Nan::New<Int32>(value));
}

Local<Value> XmlSyntaxError::BuildSyntaxError(xmlError* error) {
  Nan::EscapableHandleScope scope;

  Local<Value> err = Exception::Error(
    Nan::New<String>(error->message).ToLocalChecked());
  Local<Object> out = Local<Object>::Cast(err);

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

uint XmlSyntaxError::maxError {100};

void XmlSyntaxError::ChangeMaxNumberOfError(uint max){
  XmlSyntaxError::maxError = max;
}

void
XmlSyntaxError::PushToArray(void* errs, xmlError* error) {
  Nan::HandleScope scope;
  Local<Array> errors = *reinterpret_cast<Local<Array>*>(errs);
	uint len = errors->Length();
  if(len >= maxError){
    return;
  }
  // Local<Function> push = Local<Function>::Cast(errors->Get(Nan::New<String>("push").ToLocalChecked()));
  // Local<Value> argv[1] = { XmlSyntaxError::BuildSyntaxError(error) };
  // push->Call(errors, 1, argv);
	errors->Set(len, XmlSyntaxError::BuildSyntaxError(error));
}