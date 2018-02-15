/**
* This is the main Class script, We use NAN as a wrapper to libxml2 c+++ functions
*/
#include "libxml.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <assert.h>

using namespace std;
using namespace v8;

void Libxml::errorsHandler(void * userData, xmlErrorPtr error){
  return;
};

Libxml::Libxml() {
  //Maybe deport debug activation here ? in the future ..
}
//Destructor 
Libxml::~Libxml() {
  // Free memory dtd & xml file ..
  // If dtd is already null, just do nothing
  if(Libxml::docPtr != NULL){
     // Force clear memory DOC XML loaded
    xmlFreeDoc(Libxml::docPtr);
    Libxml::docPtr = NULL;
  }
}

Nan::Persistent<v8::Function>& Libxml::constructor() {
  static Nan::Persistent<v8::Function> myConstructor;
  return myConstructor;
}

NAN_MODULE_INIT(Libxml::Init) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(Libxml::New);
  tpl->SetClassName(Nan::New("Libxml").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "loadXml", loadXml);
  Nan::SetPrototypeMethod(tpl, "loadDtds", loadDtds);
  Nan::SetPrototypeMethod(tpl, "loadSchemas", loadSchemas);
  Nan::SetPrototypeMethod(tpl, "validateAgainstDtds", validateAgainstDtds);
  Nan::SetPrototypeMethod(tpl, "validateAgainstSchemas", validateAgainstSchemas);
  Nan::SetPrototypeMethod(tpl, "getDtd", getDtd);
  Nan::SetPrototypeMethod(tpl, "xpathSelect", xpathSelect);
  Nan::SetPrototypeMethod(tpl, "freeXml", freeXml);
  Nan::SetPrototypeMethod(tpl, "freeDtds", freeDtds);
  Nan::SetPrototypeMethod(tpl, "freeSchemas", freeSchemas);
  Nan::SetPrototypeMethod(tpl, "clearAll", clearAll);

  Local<ObjectTemplate> instTpl = tpl->InstanceTemplate();
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("Libxml").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(Libxml::New) {
  if (info.IsConstructCall()) {
    // this functions is important if we use multithreading !
    xmlInitParser();
    Libxml* obj = new Libxml();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 1;
    Local<Value> argv[argc] = {info[0]};
    Local<Function> cons = Nan::New(constructor());
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

NAN_METHOD(Libxml::loadXml) {
  if (info.Length() < 1){
    return Nan::ThrowTypeError("loadXml requires at least 1 argument");
  }
  v8::Local<v8::Array> errors = Nan::New<v8::Array>();
  xmlResetLastError();
  xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
            XmlSyntaxError::PushToArray);
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  String::Utf8Value val(info[0]->ToString());
  const char* filename(*val);
  // Those options shoudl be send by the user, it enable/disbale errors, warnings ..
  int options;
  options = (XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET);
  if(libxml->docPtr != NULL){
    xmlFreeDoc(libxml->docPtr);
    libxml->docPtr = nullptr;
  }
  libxml->docPtr = xmlReadFile(filename, NULL, options);
  xmlSetStructuredErrorFunc(NULL, NULL);

  if(libxml->docPtr == NULL){
    // We set property to libxml element only if notWellformed
    info.Holder()->Set(Nan::New<v8::String>("wellformedErrors").ToLocalChecked(), errors);
    info.GetReturnValue().Set(Nan::False());
  }else{
    info.GetReturnValue().Set(Nan::True());
  }
}

NAN_METHOD(Libxml::getDtd){
  Nan::HandleScope scope;
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  assert(libxml);
  xmlDtdPtr dtd = xmlGetIntSubset(libxml->docPtr);
  if (!dtd) {
    return info.GetReturnValue().Set(Nan::Null());
  }
  const char* name = (const char *)dtd->name;
  const char* extId = (const char *)dtd->ExternalID;
  const char* sysId = (const char *)dtd->SystemID;
  v8::Local<v8::Object> dtdObj = Nan::New<v8::Object>();
  v8::Local<v8::Value> nameValue = (v8::Local<v8::Value>)Nan::Null();
  v8::Local<v8::Value> extValue = (v8::Local<v8::Value>)Nan::Null();
  v8::Local<v8::Value> sysValue = (v8::Local<v8::Value>)Nan::Null();

  if (name != NULL) {
    nameValue = (v8::Local<v8::Value>)Nan::New<v8::String>(name, strlen(name)).ToLocalChecked();
  }
  if (extId != NULL) {
    extValue = (v8::Local<v8::Value>)Nan::New<v8::String>(extId, strlen(extId)).ToLocalChecked();
  }
  if (sysId != NULL) {
    sysValue = (v8::Local<v8::Value>)Nan::New<v8::String>(sysId, strlen(sysId)).ToLocalChecked();
  }
  // to get publicId or systemId it's the same ... http://xmlsoft.org/html/libxml-tree.html#xmlDtd
  Nan::Set(dtdObj, Nan::New<v8::String>("name").ToLocalChecked(), nameValue);
  Nan::Set(dtdObj, Nan::New<v8::String>("externalId").ToLocalChecked(), extValue);
  Nan::Set(dtdObj, Nan::New<v8::String>("systemId").ToLocalChecked(), sysValue);
  return info.GetReturnValue().Set(dtdObj);
}

NAN_METHOD(Libxml::loadDtds){
  if (info.Length() < 1){
    return Nan::ThrowTypeError("loadDtds requires at least 1 argument, an array of DTDs");
  }
  if(!info[0]->IsArray()){
    return Nan::ThrowTypeError("loadDtds requires an array");
  }
  Nan::EscapableHandleScope scope;
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  Local<Array> dtdsPathsLocal = Local<Array>::Cast(info[0]);
  Local<Array> errors = Nan::New<Array>();
  // disbale errors erreurs etc
  //v8::Local<v8::Array> errorsDTD = Nan::New<v8::Array>();
  xmlResetLastError();
  xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
            XmlSyntaxError::PushToArray);
  for (unsigned int i = 0; i < dtdsPathsLocal->Length(); i++){
    if (Nan::Has(dtdsPathsLocal, i).FromJust() && Nan::Get(dtdsPathsLocal, i).ToLocalChecked()->IsString()) {
      Local<String> value = Nan::Get(dtdsPathsLocal, i).ToLocalChecked()->ToString();
      String::Utf8Value pathV8(value->ToString());
      string pathStr (*pathV8);
      const char* path (*pathV8);
      xmlChar* pathDTDCasted = xmlCharStrdup(path);
      xmlDtdPtr dtd =  xmlParseDTD(NULL, pathDTDCasted);
      if (dtd == NULL) {
        //DTD is bad, we set error and not assign it + next
        errors->Set(i, Nan::New<String>(pathStr.c_str()).ToLocalChecked());
        continue;
      }
      libxml->dtdsPaths.push_back(dtd);
    }
  }
  xmlSetStructuredErrorFunc(NULL, NULL);
  // We set dtdsLoadedErrors property for js side
  if(errors->Length()){
    info.Holder()->Set(Nan::New<v8::String>("dtdsLoadedErrors").ToLocalChecked(), errors);
  }
}

NAN_METHOD(Libxml::loadSchemas){
  if (info.Length() < 1){
    return Nan::ThrowTypeError("loadSchemas requires at least 1 argument, an array of Schemas");
  }
  if(!info[0]->IsArray()){
    return Nan::ThrowTypeError("loadSchemas requires an array");
  }
  Nan::EscapableHandleScope scope;
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  Local<Array> schemasPathsLocal = Local<Array>::Cast(info[0]);
  Local<Array> errors = Nan::New<Array>();
  // disbale errors erreurs etc
  //v8::Local<v8::Array> errorsDTD = Nan::New<v8::Array>();
  xmlResetLastError();
  xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
            XmlSyntaxError::PushToArray);
  for (unsigned int i = 0; i < schemasPathsLocal->Length(); i++){
    if (Nan::Has(schemasPathsLocal, i).FromJust() && Nan::Get(schemasPathsLocal, i).ToLocalChecked()->IsString()) {
      Local<String> value = Nan::Get(schemasPathsLocal, i).ToLocalChecked()->ToString();
      String::Utf8Value pathV8(value->ToString());
      string pathStr (*pathV8);
      const char* path (*pathV8);
      xmlSchemaParserCtxtPtr pctxt;
      xmlSchemaPtr schema;
      // If cannot create Parse schema, just continue
      if ((pctxt = xmlSchemaNewParserCtxt(path)) == NULL) {
        errors->Set(i, Nan::New<String>(pathStr.c_str()).ToLocalChecked());
        continue;
      }
      // Chargement du contenu du XML Schema
      schema = xmlSchemaParse(pctxt);
      xmlSchemaFreeParserCtxt(pctxt);
      if (schema == NULL) {
        errors->Set(i, Nan::New<String>(pathStr.c_str()).ToLocalChecked());
        continue;
      }
      libxml->schemasPaths.push_back(schema);
    }
  }
  xmlSetStructuredErrorFunc(NULL, NULL);
  // We set dtdLoadedErrors property for js side
  if(errors->Length()){
    info.Holder()->Set(Nan::New<v8::String>("schemasLoadedErrors").ToLocalChecked(), errors);
  }
}

NAN_METHOD(Libxml::validateAgainstDtds){
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());

  if(libxml->dtdsPaths.empty()){
    info.GetReturnValue().Set(Nan::Null());
    return;
  }

  if(int newMaxNbError = info[0]->IntegerValue()){
    XmlSyntaxError::ChangeMaxNumberOfError(newMaxNbError);
  }

  //Setting contexte of validation 
  const char* dtdValidationErrorsPath;
  bool oneOfTheDtdValidate = false;
  string dtdValidateName;


  //If length 0, return null; to implement
  Local<Object> errorsValidations = Nan::New<Object>();

  for (vector<xmlDtdPtr>::iterator dtd = libxml->dtdsPaths.begin(); dtd != libxml->dtdsPaths.end() ; ++dtd){
    const char* dtdName = (const char *)(*dtd)->SystemID;
    //Local<Array> errors = New<v8::Array>(3);
    v8::Local<v8::Array> errors = Nan::New<v8::Array>();
    xmlResetLastError();
    xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
              XmlSyntaxError::PushToArray);
    Local<String> SystemIDString = Nan::New<String>(dtdName).ToLocalChecked();
    // Création du contexte de validation
    xmlValidCtxtPtr vctxt;
    //vctxt = xmlNewValidCtxt();
    if ((vctxt = xmlNewValidCtxt()) == NULL) {
      continue;
    }
    //Instead we could set this to disable output : xmlSetStructuredErrorFunc(vctxt,errorsHandler);
    vctxt->userData = (void *) Libxml::errorsHandler;
    vctxt->error = (xmlValidityErrorFunc) Libxml::errorsHandler;
    vctxt->warning = (xmlValidityWarningFunc) Libxml::errorsHandler;
    // Validation
    int result = xmlValidateDtd(vctxt, libxml->docPtr, *dtd);
    // Libération des erreurs etc
    xmlSetStructuredErrorFunc(NULL, NULL);
    xmlFreeValidCtxt(vctxt);
    if(result != 0){
      oneOfTheDtdValidate = true;
      dtdValidateName = dtdName;
      break;
    }
    errorsValidations->Set(SystemIDString, errors);
    dtdValidationErrorsPath = dtdName;
  }
  if(oneOfTheDtdValidate){
    if(dtdValidateName.length()){
      info.GetReturnValue().Set(Nan::New<v8::String>(dtdValidateName).ToLocalChecked());
    }else{
      info.GetReturnValue().Set(Nan::True());
    }
  }else{
    info.Holder()->Set(Nan::New<v8::String>("validationDtdErrors").ToLocalChecked(), errorsValidations);
    info.GetReturnValue().Set(Nan::False());
  }
}

NAN_METHOD(Libxml::validateAgainstSchemas){
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());

  if(libxml->schemasPaths.empty()){
    info.GetReturnValue().Set(Nan::Null());
    return;
  }

  if(int newMaxNbError = info[0]->IntegerValue()){
    XmlSyntaxError::ChangeMaxNumberOfError(newMaxNbError);
  }
  //Setting contexte of validation 
  const char* schemaValidationErrorsPath;
  bool oneOfTheSchemaValidate = false;
  string schemaValidateName;


  //If length 0, return null; to implement
  Local<Object> errorsValidations = Nan::New<Object>();

  for (vector<xmlSchemaPtr>::iterator xsd = libxml->schemasPaths.begin(); xsd != libxml->schemasPaths.end() ; ++xsd){
    v8::Local<v8::Array> errors = Nan::New<v8::Array>();
    xmlResetLastError();
    xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
            XmlSyntaxError::PushToArray);
    const char* xsdName = (const char *)(*xsd)->doc->URL;
    Local<String> urlSchema = Nan::New<String>(xsdName).ToLocalChecked();
    // Création du contexte de validation
    xmlSchemaValidCtxtPtr vctxt;
    if ((vctxt = xmlSchemaNewValidCtxt(*xsd)) == NULL) {
      continue;
    }
    //Instead we could set this to disable output : xmlSetStructuredErrorFunc(vctxt,errorsHandler);
    xmlSchemaSetValidErrors(vctxt, (xmlSchemaValidityErrorFunc) Libxml::errorsHandler, (xmlSchemaValidityWarningFunc) Libxml::errorsHandler, (void *) Libxml::errorsHandler);
    int result = xmlSchemaValidateDoc(vctxt, libxml->docPtr);
    // Libération des erreurs etc
    xmlSetStructuredErrorFunc(NULL, NULL);
    xmlSchemaFreeValidCtxt(vctxt);
    if(result == 0){
      oneOfTheSchemaValidate = true;
      schemaValidateName = xsdName;
      break;
    }
    errorsValidations->Set(urlSchema, errors);
    schemaValidationErrorsPath = xsdName;
  }
  if(oneOfTheSchemaValidate){
    if(schemaValidateName.length()){
      info.GetReturnValue().Set(Nan::New<v8::String>(schemaValidateName).ToLocalChecked());
    }else{
      info.GetReturnValue().Set(Nan::True());
    }
  }else{
    info.Holder()->Set(Nan::New<v8::String>("validationSchemaErrors").ToLocalChecked(), errorsValidations);
    info.GetReturnValue().Set(Nan::False());
  }
}


NAN_METHOD(Libxml::xpathSelect){
  if (info.Length() < 1){
    return Nan::ThrowTypeError("xpathSelect requires at least 1 argument");
  }
  Nan::EscapableHandleScope scope;
  v8::Local<v8::Value> res;
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  String::Utf8Value val(info[0]->ToString());
  const char* xpathToGet(*val);
  xmlChar * xpathExpr = xmlCharStrdup(xpathToGet);
  xmlXPathContextPtr xpathCtx;
  xmlXPathObjectPtr xpathObj;
  /* Create xpath evaluation context */
  xpathCtx = xmlXPathNewContext(libxml->docPtr);
  if(xpathCtx == NULL) {
    return Nan::ThrowTypeError("Error: unable to create new XPath context");
  }
  /* Evaluate xpath expression */
  xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
  if(xpathObj == NULL) {
    xmlXPathFreeContext(xpathCtx);
    info.GetReturnValue().Set(Nan::False());
  }
  else if(xpathObj) {
    switch (xpathObj->type) {
    case XPATH_BOOLEAN:
      res = Nan::New<v8::Boolean>(xpathObj->boolval);
      break;
    case XPATH_NUMBER:
      res = Nan::New<v8::Number>(xpathObj->floatval);
      break;
    case XPATH_STRING:
      res = Nan::New<v8::String>((const char *)xpathObj->stringval,
        xmlStrlen(xpathObj->stringval)).ToLocalChecked();
      break;
    default:
      res = Nan::Null();
      break;
    }
  }
  xmlXPathFreeObject(xpathObj);
  xmlXPathFreeContext(xpathCtx);
  return info.GetReturnValue().Set(scope.Escape(res));
}

NAN_METHOD(Libxml::freeXml){
  Nan::HandleScope scope;
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  // Delete Javascript property
  bool deleted = Nan::Delete(info.Holder(), Nan::New<v8::String>("wellformedErrors").ToLocalChecked()).FromMaybe(false);
  // If doc is already null, just do nothing and only available on manual mod
  if(libxml->docPtr == NULL){
    return;
  }
  // Force clear memory DTD loaded
  xmlFreeDoc(libxml->docPtr);
  libxml->docPtr = NULL;
};

NAN_METHOD(Libxml::freeDtds){
  Nan::HandleScope scope;
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  // Delete Javascript property
  bool deletedLoaded = Nan::Delete(info.Holder(), Nan::New<v8::String>("dtdsLoadedErrors").ToLocalChecked()).FromMaybe(false);
  bool deleted = Nan::Delete(info.Holder(), Nan::New<v8::String>("validationDtdErrors").ToLocalChecked()).FromMaybe(false);
  // If dtds is already empty, just stop here
  if(libxml->dtdsPaths.empty()){
    return;
  }
  for (vector<xmlDtdPtr>::iterator dtd = libxml->dtdsPaths.begin(); dtd != libxml->dtdsPaths.end() ; ++dtd){
    if(*dtd != NULL){
      // Force clear memory DTD loaded
      xmlFreeDtd(*dtd);
      *dtd = nullptr;
    }
  }
  // clear the vector of dtds
  libxml->dtdsPaths.clear();
}

NAN_METHOD(Libxml::freeSchemas){
  Nan::HandleScope scope;
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  // Delete Javascript property
  bool deletedLoaded = Nan::Delete(info.Holder(), Nan::New<v8::String>("schemasLoadedErrors").ToLocalChecked()).FromMaybe(false);
  bool deleted = Nan::Delete(info.Holder(), Nan::New<v8::String>("validationSchemasErrors").ToLocalChecked()).FromMaybe(false);
  // If dtds is already empty, just stop here
  if(libxml->schemasPaths.empty()){
    return;
  }
  for (vector<xmlSchemaPtr>::iterator xsd = libxml->schemasPaths.begin(); xsd != libxml->schemasPaths.end() ; ++xsd){
    if(*xsd != NULL){
      // Force clear memory xsd loaded
      xmlSchemaFree(*xsd);
      *xsd = nullptr;
    }
  }
  // clear the vector of dtds
  libxml->schemasPaths.clear();
}

NAN_METHOD(Libxml::clearAll){
  Nan::HandleScope scope;
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  // Attention this function clear all the libxml2 memory for all threads!
  xmlCleanupParser();
}

void InitLibxml(v8::Local<v8::Object> exports) { Libxml::Init(exports); }

NODE_MODULE(libxml, InitLibxml);
