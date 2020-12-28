/**
* This is the main Class script, We use NODE-ADDON-API (N-API) as a wrapper to libxml2 c++ functions
*/
#include "libxml.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <assert.h>

Napi::Object Libxml::Init(Napi::Env env, Napi::Object exports)
{
  // This method is used to hook the accessor and method callbacks
  Napi::Function func = DefineClass(env, "Libxml", {
    InstanceMethod("loadXml", &Libxml::loadXml),
    InstanceMethod("freeXml", &Libxml::freeXml)
    });

  // Create a peristent reference to the class constructor. This will allow
  // a function called on a class prototype and a function
  // called on instance of a class to be distinguished from each other.
  constructor = Napi::Persistent(func);
  // Call the SuppressDestruct() method on the static data prevent the calling
  // to this destructor to reset the reference when the environment is no longer
  // available.
  constructor.SuppressDestruct();
  exports.Set("Libxml", func);
  return exports;
}

Libxml::Libxml(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Libxml>(info)
{
  Napi::Env env = info.Env();
  xmlInitParser();
}

Napi::FunctionReference Libxml::constructor;

Napi::Value Libxml::loadXml(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "loadXml requires at least 1 argument").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  cout << "before libxml" << endl;
  int options;
  options = (XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET);
  cout << "after libxml 1" << endl;
  Napi::Array errors = Napi::Array::New(env);
  xmlResetLastError();
  XmlSyntaxError::env = &env;
  xmlSetStructuredErrorFunc(reinterpret_cast<void*>(&errors),
                            XmlSyntaxError::PushToArray);
  if (this->docPtr != NULL)
  {
    cout << "after libxml 2" << endl;
    xmlFreeDoc(this->docPtr);
    this->docPtr = nullptr;
  }
  cout << "after libxml 3" << endl;
  Napi::String path = info[0].As<Napi::String>();
  this->path = path.ToString();
  cout << "after libxml 4" << endl;
  cout << this->path.c_str() << endl;
  const char *pathToRead = this->path.c_str();
  this->docPtr = xmlReadFile(pathToRead, NULL, options);
  cout << "after libxml 5" << endl;
  xmlSetStructuredErrorFunc(NULL, NULL);
  cout << "after libxml 6" << endl;
  if (this->docPtr == NULL)
  {
    // return Napi::Array(errors);
    this->Value().Set("wellformedErrors", errors);
    return Napi::Boolean::New(env, false);
  } else {
    this->Value().Delete("wellformedErrors");
  }
  return Napi::Boolean::New(env, true);
}

void Libxml::freeXml(const Napi::CallbackInfo& info) {
  Napi::HandleScope scope(info.Env());
  // Delete Javascript property
  this->Value().Delete("wellformedErrors");
  // If doc is already null, just do nothing and only available on manual mod
  if(this->docPtr == nullptr){
    return;
  }
  // Force clear the memory loaded for XML
  xmlFreeDoc(this->docPtr);
  this->docPtr = nullptr;
}

// Initialize native add-on
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  Libxml::Init(env, exports);
  return exports;
}

// Register and initialize native add-on
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)