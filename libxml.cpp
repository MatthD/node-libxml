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
   // if(!debug){
   //  void *  vctxt;
   //  xmlSetStructuredErrorFunc(vctxt,errorsHandler);
   // }
}
Libxml::~Libxml() {}

Nan::Persistent<v8::Function>& Libxml::constructor() {
  static Nan::Persistent<v8::Function> myConstructor;
  return myConstructor;
}

NAN_MODULE_INIT(Libxml::Init) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(Libxml::New);
  tpl->SetClassName(Nan::New("Libxml").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "load", load);
  Nan::SetPrototypeMethod(tpl, "getDtd", getDtd);
  Nan::SetPrototypeMethod(tpl, "xpathSelect", xpathSelect);
  Nan::SetPrototypeMethod(tpl, "validateAgainstDtd", validateAgainstDtd);

  Local<ObjectTemplate> instTpl = tpl->InstanceTemplate();

  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("Libxml").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(Libxml::New) {
  if (info.IsConstructCall()) {
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

NAN_METHOD(Libxml::load) {
  if (info.Length() < 1){
    return Nan::ThrowTypeError("Load requires at least 1 argument");
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
  libxml->docPrt = xmlReadFile(filename, NULL, options);

  xmlSetStructuredErrorFunc(NULL, NULL);

  if(libxml->docPrt == NULL){
    // We set property to libxml element only if notWellformed
    info.Holder()->Set(Nan::New<v8::String>("wellformedErrors").ToLocalChecked(), errors);
    info.GetReturnValue().Set(Nan::False());
  }else{
    info.GetReturnValue().Set(Nan::True());
  }
}

NAN_METHOD(Libxml::getDtd){
  Nan::HandleScope scope;
  Libxml* document = Nan::ObjectWrap::Unwrap<Libxml>(info.Holder());
  assert(document);
  xmlDtdPtr dtd = xmlGetIntSubset(document->docPrt);

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
    return Nan::ThrowTypeError("Load requires at least 1 argument");
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
  xmlDtdPtr dtdPtr;
  xmlValidCtxtPtr vctxt;
  xmlChar * fichierDTDCasted = xmlCharStrdup(filenameDtd);

  if ((dtdPtr = xmlParseDTD(NULL, fichierDTDCasted)) == NULL) {
    return Nan::ThrowTypeError("ERROR_OCCURED, DTD file is not valid");
  }
  // Création du contexte de validation
  if ((vctxt = xmlNewValidCtxt()) == NULL) {
    xmlFreeDtd(dtdPtr);
    return Nan::ThrowTypeError("ERROR_OCCURED, cannot create validation contexte");
  }

  //Instead we could set this to disable output : xmlSetStructuredErrorFunc(vctxt,errorsHandler);
  if (!showErrors) {
    vctxt->userData = (void *) Libxml::errorsHandler;
    vctxt->error = (xmlValidityErrorFunc) Libxml::errorsHandler;
    vctxt->warning = (xmlValidityWarningFunc) Libxml::errorsHandler;
  }

  // Validation
  dtdValidationResult = xmlValidateDtd(vctxt, libxml->docPrt, dtdPtr);

  // Libération de la mémoire, erreurs etc
  xmlSetStructuredErrorFunc(NULL, NULL);
  xmlFreeValidCtxt(vctxt);
  xmlFreeDtd(dtdPtr);

  if(dtdValidationResult != 0){
    info.GetReturnValue().Set(Nan::True());
  }else{
    info.Holder()->Set(Nan::New<v8::String>("validationErrors").ToLocalChecked(), errors);
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
  xpathCtx = xmlXPathNewContext(libxml->docPrt);
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

void InitLibxml(v8::Local<v8::Object> exports) { Libxml::Init(exports); }

NODE_MODULE(libxml, InitLibxml);
