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
    InstanceMethod("loadSchemas", &Libxml::loadSchemas),
    InstanceMethod("validateAgainstDtds", &Libxml::validateAgainstDtds),
    InstanceMethod("validateAgainstSchemas", &Libxml::validateAgainstSchemas),
    InstanceMethod("xpathSelect", &Libxml::xpathSelect),
    InstanceMethod("getDtd", &Libxml::getDtd),
    InstanceMethod("freeXml", &Libxml::freeXml),
    InstanceMethod("freeDtds", &Libxml::freeDtds),
    InstanceMethod("freeSchemas", &Libxml::freeSchemas),
    InstanceMethod("clearAll", &Libxml::clearAll)
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
  int options;
  options = (XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET);
  Napi::Array errors = Napi::Array::New(env);
  xmlResetLastError();
  XmlSyntaxError::env = &env;
  xmlSetStructuredErrorFunc(reinterpret_cast<void*>(&errors),
                            XmlSyntaxError::PushToArray);
  if (this->docPtr != nullptr)
  {
    xmlFreeDoc(this->docPtr);
    this->docPtr = nullptr;
  }
  Napi::String path = info[0].As<Napi::String>();
  this->path = path.ToString();
  const char *pathToRead = this->path.c_str();
  this->docPtr = xmlReadFile(pathToRead, NULL, options);
  xmlSetStructuredErrorFunc(NULL, NULL);
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

Napi::Value Libxml::loadSchemas(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1){
     Napi::TypeError::New(env, "loadSchemas requires at least 1 argument, an array of Schemas").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if(!info[0].IsArray()){
    Napi::TypeError::New(env, "loadSchemas requires an array").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  Napi::EscapableHandleScope scope(env);
  Napi::Array schemasPathsLocal = info[0].As<Napi::Array>();
  //set up error handlers
  Napi::Array errors = Napi::Array::New(env);
  xmlResetLastError();
  XmlSyntaxError::env = &env;
  xmlSetStructuredErrorFunc(reinterpret_cast<void*>(&errors),
                          XmlSyntaxError::PushToArray);
  for (unsigned int i = 0; i < schemasPathsLocal.Length(); i++){
    // Handle value if string and drop it silently otherwise
    if(schemasPathsLocal.Get(i).IsString()) {
      Napi::String value = schemasPathsLocal.Get(i).As<Napi::String>();
      string pathStr (value.Utf8Value());
      const char* path (pathStr.c_str());
      xmlSchemaParserCtxtPtr pctxt;
      xmlSchemaPtr schema;
      // If cannot create Parse schema, just continue
      if ((pctxt = xmlSchemaNewParserCtxt(path)) == NULL) {
        XmlSyntaxError::PushToArray(errors, path);
        continue;
      }
      // Loading XML Schema content
      schema = xmlSchemaParse(pctxt);
      xmlSchemaFreeParserCtxt(pctxt);
      if (schema == nullptr) {
        XmlSyntaxError::PushToArray(errors, path);
        continue;
      }
      this->schemasPaths.push_back(schema);
    }
  }
  xmlSetStructuredErrorFunc(NULL, NULL);
  // We set dtdLoadedErrors property for js side
  if(errors.Length()){
    this->Value().Set("schemasLoadedErrors", errors);
  } else {
    this->Value().Delete("schemasLoadedErrors");
  }
  return env.Undefined();
}

Napi::Value Libxml::validateAgainstDtds(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if(this->dtdsPaths.empty()){
    return env.Null();
  }

  // if first param is number then apply it. If not then silently drop it 
  if(info.Length() > 0 && info[0].IsNumber()) {
    XmlSyntaxError::ChangeMaxNumberOfError(info[0].ToNumber());
  }

  //Setting context of validation 
  bool oneOfTheDtdValidate = false;
  string dtdValidateName;


  //If length 0, return null; to implement
  Napi::Object errorsValidations = Napi::Object::New(env);

  for (vector<xmlDtdPtr>::iterator dtd = this->dtdsPaths.begin(); dtd != this->dtdsPaths.end() ; ++dtd){
    const char* dtdName = (const char *)(*dtd)->SystemID;

    //set up error handling
    Napi::Array errors = Napi::Array::New(env);
    xmlResetLastError();
    XmlSyntaxError::env = &env;
    xmlSetStructuredErrorFunc(reinterpret_cast<void*>(&errors),
                            XmlSyntaxError::PushToArray);
    
    Napi::String SystemIDString = Napi::String::New(env, dtdName);
    // Context creation for validation
    xmlValidCtxtPtr vctxt;
    if ((vctxt = xmlNewValidCtxt()) == nullptr) {
      continue;
    }
    //Instead we could set this to disable output : xmlSetStructuredErrorFunc(vctxt,errorsHandler);
    //xmlSetStructuredErrorFunc(vctxt, nullptr);

    vctxt->userData = nullptr;
    vctxt->error = nullptr;
    vctxt->warning = nullptr;

    // Validation
    int result = xmlValidateDtd(vctxt, this->docPtr, *dtd);

    // drop error handling function and free context
    xmlSetStructuredErrorFunc(nullptr, nullptr);
    xmlFreeValidCtxt(vctxt);
    //If validation was successfull than break the loop
    if(result != 0){
      oneOfTheDtdValidate = true;
      dtdValidateName = dtdName;
      break;
    }
    // If validation failed add result to error object
    errorsValidations.Set(SystemIDString, errors);
  }
  if(oneOfTheDtdValidate){
    this->Value().Delete("validationDtdErrors");
    if(dtdValidateName.length()){
      return Napi::String::New(env, dtdValidateName);
    }else{
      return Napi::Boolean::New(env, true);
    }
  }else{
    this->Value().Set("validationDtdErrors", errorsValidations);
    return Napi::Boolean::New(env, false);
  }
}

Napi::Value Libxml::validateAgainstSchemas(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if(this->schemasPaths.empty()){
    return env.Null();;
  }

  if(info[0].IsNumber()){
    XmlSyntaxError::ChangeMaxNumberOfError(info[0].ToNumber().Int32Value());
  }
  //Setting context of validation 
  const char* schemaValidationErrorsPath;
  bool oneOfTheSchemaValidate = false;
  string schemaValidateName;


  //If length 0, return null; to implement
  //Local<Object> errorsValidations = Nan::New<Object>();
  Napi::Object errorsValidations = Napi::Object::New(env);

  for (vector<xmlSchemaPtr>::iterator xsd = this->schemasPaths.begin(); xsd != this->schemasPaths.end() ; ++xsd){
    //set up error handling
    Napi::Array errors = Napi::Array::New(env);
    xmlResetLastError();
    XmlSyntaxError::env = &env;
    xmlSetStructuredErrorFunc(reinterpret_cast<void*>(&errors),
                            XmlSyntaxError::PushToArray);

    const char* xsdName = (const char *)(*xsd)->doc->URL;
    //Local<String> urlSchema = Nan::New<String>(xsdName).ToLocalChecked();
    Napi::String urlSchema = Napi::String::New(env, xsdName);
    // Creating the validation context
    xmlSchemaValidCtxtPtr vctxt;
    if ((vctxt = xmlSchemaNewValidCtxt(*xsd)) == nullptr) {
      continue;
    }
    //Instead we could set this to disable output : xmlSetStructuredErrorFunc(vctxt,errorsHandler);
    // xmlSchemaSetValidErrors(vctxt, (xmlSchemaValidityErrorFunc) Libxml::errorsHandler, (xmlSchemaValidityWarningFunc) Libxml::errorsHandler, (void *) Libxml::errorsHandler);
    xmlSchemaSetValidErrors(vctxt, nullptr, nullptr, nullptr);
    int result = xmlSchemaValidateDoc(vctxt, this->docPtr);
    // Stop listening for errors
    xmlSetStructuredErrorFunc(nullptr, nullptr);
    xmlSchemaFreeValidCtxt(vctxt);
    if(result == 0){
      oneOfTheSchemaValidate = true;
      schemaValidateName = xsdName;
      break;
    }
    errorsValidations.Set(urlSchema, errors);
    schemaValidationErrorsPath = xsdName;
  }
  if(oneOfTheSchemaValidate){
    this->Value().Delete("validationSchemaErrors");
    if(schemaValidateName.length()){
      //info.GetReturnValue().Set(Nan::New<v8::String>(schemaValidateName).ToLocalChecked());
      return Napi::String::New(env, schemaValidateName);
    }else{
      //info.GetReturnValue().Set(Nan::True());
      return Napi::Boolean::New(env, true);
    }
  }else{
    // info.Holder()->Set(Nan::New<v8::String>("validationSchemaErrors").ToLocalChecked(), errorsValidations);
    // info.GetReturnValue().Set(Nan::False());
    this->Value().Set("validationSchemaErrors", errorsValidations);
    return Napi::Boolean::New(env, false);
  }
}

Napi::Value Libxml::xpathSelect(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1){
    Napi::TypeError::New(env, "xpathSelect requires at least 1 argument").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  Napi::EscapableHandleScope scope(env);
  Napi::Value res;
  Napi::String val = info[0].ToString();
  //Need to keep string in memory and get its const char* casted version for libxml2
  std::string xpathStr = val.Utf8Value();
  const char* xpathToGet(xpathStr.c_str());
  xmlChar * xpathExpr = xmlCharStrdup(xpathToGet);
  xmlXPathContextPtr xpathCtx;
  xmlXPathObjectPtr xpathObj;
  /* Create xpath evaluation context */
  xpathCtx = xmlXPathNewContext(this->docPtr);
  if(xpathCtx == nullptr) {
    Napi::Error::New(env, "Error: unable to create new XPath context").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  /* Evaluate xpath expression */
  xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
  if(xpathObj == nullptr) {
    xmlXPathFreeContext(xpathCtx);
    return Napi::Boolean::New(env, false);
  }
  else if(xpathObj) {
    switch (xpathObj->type) {
    case XPATH_BOOLEAN:
      res = Napi::Boolean::New(env, xpathObj->boolval);
      break;
    case XPATH_NUMBER:
      res = Napi::Number::New(env, xpathObj->floatval);
      break;
    case XPATH_STRING:
      res = Napi::String::New(env, (const char *)xpathObj->stringval);
      break;
    default:
      res = env.Null();
      break;
    }
  }
  xmlXPathFreeObject(xpathObj);
  xmlXPathFreeContext(xpathCtx);
  return scope.Escape(res);
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

void Libxml::freeDtds(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  // Delete Javascript property
  this->Value().Delete("dtdsLoadedErrors");
  this->Value().Delete("validationDtdErrors");
  // If dtds is already empty, just stop here
  if(this->dtdsPaths.empty()){
    return;
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
  return;
}

void Libxml::freeSchemas(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  //Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  // Delete Javascript property
  // bool deletedLoaded = Nan::Delete(info.Holder(), Nan::New<v8::String>("schemasLoadedErrors").ToLocalChecked()).FromMaybe(false);
  // bool deleted = Nan::Delete(info.Holder(), Nan::New<v8::String>("validationSchemasErrors").ToLocalChecked()).FromMaybe(false);
  this->Value().Delete("schemasLoadedErrors");
  this->Value().Delete("validationSchemasErrors");
  // If dtds is already empty, just stop here
  if(this->schemasPaths.empty()){
    return;
  }
  for (vector<xmlSchemaPtr>::iterator xsd = this->schemasPaths.begin(); xsd != this->schemasPaths.end() ; ++xsd){
    if(*xsd != nullptr){
      // Force clear memory xsd loaded
      xmlSchemaFree(*xsd);
      *xsd = nullptr;
    }
  }
  // clear the vector of dtds
  this->schemasPaths.clear();
//
}

void Libxml::clearAll(const Napi::CallbackInfo& info) {
  this->freeXml(info);
  this->freeDtds(info);
  this->freeSchemas(info);
  xmlCleanupParser();
}

// Initialize native add-on
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  Libxml::Init(env, exports);
  return exports;
}

// Register and initialize native add-on
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
