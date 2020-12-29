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
    InstanceMethod("loadXmlFromString", &Libxml::loadXmlFromString),
    InstanceMethod("loadDtds", &Libxml::loadDtds),
    InstanceMethod("getDtd", &Libxml::getDtd),
    InstanceMethod("freeXml", &Libxml::freeXml),
    InstanceMethod("freeDtds", &Libxml::freeDtds)
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
  if (this->docPtr != nullptr)
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
    this->Value().Set("wellformedErrors", errors);
    return Napi::Boolean::New(env, false);
  } else {
    this->Value().Delete("wellformedErrors");
  }
  return Napi::Boolean::New(env, true);
}

Napi::Value Libxml::loadXmlFromString(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "loadXmlFromString requires at least 1 argument").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  Napi::Array errors = Napi::Array::New(env);
  xmlResetLastError();
  XmlSyntaxError::env = &env;
  xmlSetStructuredErrorFunc(reinterpret_cast<void*>(&errors),
                            XmlSyntaxError::PushToArray);
  
  // Those options should be send by the user, it enable/disbale errors, warnings ..
  int options = (XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET);
  if(this->docPtr != NULL){
    xmlFreeDoc(this->docPtr);
    this->docPtr = nullptr;
  }

  Napi::String txt = info[0].As<Napi::String>();
  std::string txtToRead = txt.Utf8Value();

  this->docPtr = xmlReadMemory(txtToRead.c_str(), strlen(txtToRead.c_str()), nullptr, nullptr, options);
  xmlSetStructuredErrorFunc(nullptr, nullptr);

  if(this->docPtr == nullptr){
    // We set property to libxml element only if notWellformed
    this->Value().Set("wellformedErrors", errors);
    return Napi::Boolean::New(env, false);
  }else{
    this->Value().Delete("wellformedErrors");
  }
  return Napi::Boolean::New(env, true);
}

Napi::Value Libxml::loadDtds(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1){
    Napi::TypeError::New(env, "loadDtds requires at least 1 argument, an array of DTDs").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if(!info[0].IsArray()){
    Napi::TypeError::New(env, "loadDtds requires an array").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  Napi::EscapableHandleScope scope(env);
  Napi::Array dtdPaths = info[0].As<Napi::Array>();
  Napi::Array errors = Napi::Array::New(env);
  xmlResetLastError();
  XmlSyntaxError::env = &env;
  xmlSetStructuredErrorFunc(reinterpret_cast<void*>(&errors),
                            XmlSyntaxError::PushToArray);
  for (unsigned int i = 0; i < dtdPaths.Length(); i++){
    //Skip elements silently which are not strings
    if(dtdPaths.Get(i).IsString()) {
      std::string dtdPath = dtdPaths.Get(i).ToString().Utf8Value();
      xmlChar* pathDTDCasted = xmlCharStrdup(dtdPath.c_str());
      xmlDtdPtr dtd =  xmlParseDTD(NULL, pathDTDCasted);
      if (dtd == nullptr) {
        //DTD is bad, we set error and not assign it
        XmlSyntaxError::PushToArray(errors, dtdPath.c_str());
        continue;
      }
      this->dtdsPaths.push_back(dtd);
    }
  }
  xmlSetStructuredErrorFunc(nullptr, nullptr);
  // We set dtdsLoadedErrors property for js side
  if(errors.Length()){
    this->Value().Set("dtdsLoadedErrors", errors);
  } else {
    this->Value().Delete("dtdsLoadedErrors");
  }
  return env.Undefined();
}

Napi::Value Libxml::getDtd(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  // Get DTD using libxml2
  xmlDtdPtr dtd = xmlGetIntSubset(this->docPtr);
  //return null if no valid xml loaded so far
  if (!dtd) {
    return env.Null();
  }

  //Set up return object.
  const char* name = (const char *)dtd->name;
  const char* extId = (const char *)dtd->ExternalID;
  const char* sysId = (const char *)dtd->SystemID;
  Napi::Object dtdObject = Napi::Object::New(env);
  Napi::Value nameValue = name
                            ? Napi::String::New(env, name)
                            : env.Null();
  Napi::Value extValue = extId
                            ? Napi::String::New(env, extId)
                            : env.Null();
  Napi::Value sysValue = sysId
                            ? Napi::String::New(env, sysId)
                            : env.Null();

  // to get publicId or systemId it's the same ... http://xmlsoft.org/html/libxml-tree.html#xmlDtd
  dtdObject.Set("name", nameValue);
  dtdObject.Set("externalId", extValue);
  dtdObject.Set("systemId", sysValue);
  return dtdObject;
}

void Libxml::freeXml(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
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

Napi::Value Libxml::freeDtds(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  // Delete Javascript property
  this->Value().Delete("dtdsLoadedErrors");
  this->Value().Delete("validationDtdErrors");
  // If dtds is already empty, just stop here
  if(this->dtdsPaths.empty()){
    return env.Undefined();
  }
  for (vector<xmlDtdPtr>::iterator dtd = this->dtdsPaths.begin(); dtd != this->dtdsPaths.end() ; ++dtd){
    if(*dtd != nullptr){
      // Force clear memory DTD loaded
      xmlFreeDtd(*dtd);
      *dtd = nullptr;
    }
  }
  // clear the vector of dtds
  this->dtdsPaths.clear();
  return env.Undefined();
}

// Initialize native add-on
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  Libxml::Init(env, exports);
  return exports;
}

// Register and initialize native add-on
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)