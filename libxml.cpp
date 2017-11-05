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

Libxml::Libxml(bool manual) {
  //Maybe deport debug activation here ? in the future ..
  if(manual){
    Libxml::manual = true;
  }
}
//Destructor 
Libxml::~Libxml() {
  // Free memory dtd & xml file ..
  xmlFreeDoc(Libxml::docPtr);
  xmlFreeDtd(Libxml::dtdPtr);
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
  Nan::SetPrototypeMethod(tpl, "getDtd", getDtd);
  Nan::SetPrototypeMethod(tpl, "xpathSelect", xpathSelect);
  Nan::SetPrototypeMethod(tpl, "validateAgainstDtd", validateAgainstDtd);
  Nan::SetPrototypeMethod(tpl, "freeDtd", freeDtd);
  Nan::SetPrototypeMethod(tpl, "freeXml", freeXml);

  Local<ObjectTemplate> instTpl = tpl->InstanceTemplate();

  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("Libxml").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(Libxml::New) {
  if (info.IsConstructCall()) {
    bool manual = info[0]->BooleanValue() || false;
    Libxml* obj = new Libxml(manual);
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

  int options;
  options = (XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET);
  libxml->docPtr = xmlReadFile(filename, NULL, options);

  xmlSetStructuredErrorFunc(NULL, NULL);

  // Free memory only in automatic mode (default), if manuel , do nothing

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

NAN_METHOD(Libxml::validateAgainstDtd){
  if (info.Length() < 1){
    return Nan::ThrowTypeError("validateAgainstDtd requires at least 1 argument");
  }
  v8::Local<v8::Array> errors = Nan::New<v8::Array>();
  xmlResetLastError();
  xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
            XmlSyntaxError::PushToArray);
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  String::Utf8Value val(info[0]->ToString());
  const char* filenameDtd(*val);
  bool showErrors = info[1]->BooleanValue();;

  int dtdValidationResult;
  xmlValidCtxtPtr vctxt;
  xmlChar * fichierDTDCasted = xmlCharStrdup(filenameDtd);

  if ((libxml->dtdPtr = xmlParseDTD(NULL, fichierDTDCasted)) == NULL) {
    return Nan::ThrowTypeError("ERROR_OCCURED, DTD file is not valid");
  }
  // Création du contexte de validation
  if ((vctxt = xmlNewValidCtxt()) == NULL) {
    xmlFreeDtd(libxml->dtdPtr);
    return Nan::ThrowTypeError("ERROR_OCCURED, cannot create validation contexte");
  }

  //Instead we could set this to disable output : xmlSetStructuredErrorFunc(vctxt,errorsHandler);
  if (!showErrors) {
    vctxt->userData = (void *) Libxml::errorsHandler;
    vctxt->error = (xmlValidityErrorFunc) Libxml::errorsHandler;
    vctxt->warning = (xmlValidityWarningFunc) Libxml::errorsHandler;
  }

  // Validation
  dtdValidationResult = xmlValidateDtd(vctxt, libxml->docPtr, libxml->dtdPtr);

  // Libération de la mémoire, erreurs etc
  xmlSetStructuredErrorFunc(NULL, NULL);
  xmlFreeValidCtxt(vctxt);

  if(dtdValidationResult != 0){
    info.GetReturnValue().Set(Nan::True());
  }else{
    info.Holder()->Set(Nan::New<v8::String>("validationErrors").ToLocalChecked(), errors);
    info.GetReturnValue().Set(Nan::False());
  }

  // Free dtd only in automatic mode, not manual
  bool isManual = libxml->manual;
  if(!isManual){
    xmlFreeDoc(libxml->docPtr);
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
  return info.GetReturnValue().Set(scope.Escape(res));
}

NAN_METHOD(Libxml::freeXml){
  Nan::HandleScope scope;
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  // If dtd is already null, just do nothing .. this is to implement
  // if(libxml->dtdPtr == NULL){
  //   return;
  // }
  // Force clear memory DTD loaded
  v8::Local<v8::Array> emptyErrors = Nan::New<v8::Array>();
  info.Holder()->Set(Nan::New<v8::String>("wellformedErrors").ToLocalChecked(), emptyErrors);
  xmlFreeDoc(libxml->docPtr);
};

NAN_METHOD(Libxml::freeDtd){
  Nan::HandleScope scope;
  Libxml* libxml = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  // If dtd is already null, just do nothing .. this is to implement
  // if(libxml->dtdPtr == NULL){
  //   return;
  // }
  // Force clear memory DTD loaded
  v8::Local<v8::Array> emptyErrors = Nan::New<v8::Array>();
  info.Holder()->Set(Nan::New<v8::String>("validationErrors").ToLocalChecked(), emptyErrors);
  xmlFreeDtd(libxml->dtdPtr);
};

void InitLibxml(v8::Local<v8::Object> exports) { Libxml::Init(exports); }

NODE_MODULE(libxml, InitLibxml);
