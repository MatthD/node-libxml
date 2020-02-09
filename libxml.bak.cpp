/**
* This is the main Class script, We use NAN as a wrapper to libxml2 c+++ functions
*/
#include "libxml.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <assert.h>

void Libxml::errorsHandler(void *userData, xmlErrorPtr error)
{
  return;
};

Libxml::Libxml()
{
  //Maybe deport debug activation here ? in the future ..
}
//Destructor
Libxml::~Libxml()
{
  // Free memory dtd & xml file ..
  // If dtd is already null, just do nothing
  if (Libxml::docPtr != NULL)
  {
    // Force clear memory DOC XML loaded
    xmlFreeDoc(Libxml::docPtr);
    Libxml::docPtr = NULL;
  }
}

Napi::FunctionReference &Libxml::constructor()
{
  static Napi::FunctionReference myConstructor;
  return myConstructor;
}

Napi::Object Libxml::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = Napi::Function::New(env, Libxml::New);
  // tpl->SetClassName(Napi::String::New(env, "Libxml"));
  Napi::Function func = DefineClass(
      env, "Libxml", {InstanceMethod("loadXml", &loadXml)});

  InstanceMethod("loadXml", &loadXml),
      InstanceMethod("loadXmlFromString", &loadXmlFromString),
      InstanceMethod("loadDtds", &loadDtds),
      // InstanceMethod("loadDtdsFromString", &loadDtdsFromString),
      InstanceMethod("loadSchemas", &loadSchemas),
      InstanceMethod("validateAgainstDtds", &validateAgainstDtds),
      InstanceMethod("validateAgainstSchemas", &validateAgainstSchemas),
      InstanceMethod("getDtd", &getDtd),
      InstanceMethod("xpathSelect", &xpathSelect),
      InstanceMethod("freeXml", &freeXml),
      InstanceMethod("freeDtds", &freeDtds),
      InstanceMethod("freeSchemas", &freeSchemas),
      InstanceMethod("clearAll", &clearAll),

      Local<ObjectTemplate> instTpl = tpl->InstanceTemplate();
  constructor().Reset(Napi::GetFunction(tpl));
  (target).Set(Napi::String::New(env, "Libxml"), tpl->GetFunction());
}

Napi::Value Libxml::New(const Napi::CallbackInfo &info)
{
  if (info.IsConstructCall())
  {
    // this functions is important if we use multithreading !
    xmlInitParser();
    Libxml *obj = new Libxml();
    obj->Wrap(info.This());
    return info.This();
  }
  else
  {
    const int argc = 1;
    Napi::Value argv[argc] = {info[0]};
    Napi::Function cons = Napi::New(env, constructor());
    return Napi::NewInstance(cons, argc, argv);
  }
}

Napi::Value Libxml::loadXml(const Napi::CallbackInfo &info)
{
  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "loadXml requires at least 1 argument").ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::Array errors = Napi::Array::New(env);
  xmlResetLastError();
  xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
                            XmlSyntaxError::PushToArray);
  Libxml *libxml = info.Holder().Unwrap<Libxml>();
  Napi::String val(env, info[0].ToString());
  const char *filename(*val);
  // Those options shoudl be send by the user, it enable/disbale errors, warnings ..
  int options;
  options = (XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET);
  if (libxml->docPtr != NULL)
  {
    xmlFreeDoc(libxml->docPtr);
    libxml->docPtr = nullptr;
  }
  libxml->docPtr = xmlReadFile(filename, NULL, options);
  xmlSetStructuredErrorFunc(NULL, NULL);

  if (libxml->docPtr == NULL)
  {
    // We set property to libxml element only if notWellformed
    info.Holder().Set(Napi::String::New(env, "wellformedErrors"), errors);
    return env.False();
  }
  else
  {
    return env.True();
  }
}

Napi::Value Libxml::loadXmlFromString(const Napi::CallbackInfo &info)
{
  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "loadXmlFromString requires at least 1 argument").ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::Array errors = Napi::Array::New(env);
  xmlResetLastError();
  xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
                            XmlSyntaxError::PushToArray);
  Libxml *libxml = info.Holder().Unwrap<Libxml>();
  Napi::String str(env, info[0].ToString());
  string txt(*str);
  // Those options shoudl be send by the user, it enable/disbale errors, warnings ..
  int options;
  options = (XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET);
  if (libxml->docPtr != NULL)
  {
    xmlFreeDoc(libxml->docPtr);
    libxml->docPtr = nullptr;
  }
  libxml->docPtr = xmlReadMemory(txt.c_str(), txt.Length(), NULL, NULL, options);
  xmlSetStructuredErrorFunc(NULL, NULL);

  if (libxml->docPtr == NULL)
  {
    // We set property to libxml element only if notWellformed
    info.Holder().Set(Napi::String::New(env, "wellformedErrors"), errors);
    return env.False();
  }
  else
  {
    return env.True();
  }
}

Napi::Value Libxml::getDtd(const Napi::CallbackInfo &info)
{
  Napi::HandleScope scope(env);
  Libxml *libxml = info.Holder().Unwrap<Libxml>();
  assert(libxml);
  xmlDtdPtr dtd = xmlGetIntSubset(libxml->docPtr);
  if (!dtd)
  {
    return return env.Null();
  }
  const char *name = (const char *)dtd->name;
  const char *extId = (const char *)dtd->ExternalID;
  const char *sysId = (const char *)dtd->SystemID;
  Napi::Object dtdObj = Napi::Object::New(env);
  Napi::Value nameValue = (Napi::Value)env.Null();
  Napi::Value extValue = (Napi::Value)env.Null();
  Napi::Value sysValue = (Napi::Value)env.Null();

  if (name != NULL)
  {
    nameValue = (Napi::Value)Napi::String::New(env, name, strlen(name));
  }
  if (extId != NULL)
  {
    extValue = (Napi::Value)Napi::String::New(env, extId, strlen(extId));
  }
  if (sysId != NULL)
  {
    sysValue = (Napi::Value)Napi::String::New(env, sysId, strlen(sysId));
  }
  // to get publicId or systemId it's the same ... http://xmlsoft.org/html/libxml-tree.html#xmlDtd
  (dtdObj).Set(Napi::String::New(env, "name"), nameValue);
  (dtdObj).Set(Napi::String::New(env, "externalId"), extValue);
  (dtdObj).Set(Napi::String::New(env, "systemId"), sysValue);
  return return dtdObj;
}

Napi::Value Libxml::loadDtds(const Napi::CallbackInfo &info)
{
  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "loadDtds requires at least 1 argument, an array of DTDs").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!info[0].IsArray())
  {
    Napi::TypeError::New(env, "loadDtds requires an array").ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::EscapableHandleScope scope(env);
  Libxml *libxml = info.Holder().Unwrap<Libxml>();
  Napi::Array dtdsPathsLocal = info[0].As<Napi::Array>();
  Napi::Array errors = Napi::Array::New(env);
  // disbale errors erreurs etc
  //Napi::Array errorsDTD = Napi::Array::New(env);
  xmlResetLastError();
  xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
                            XmlSyntaxError::PushToArray);
  for (unsigned int i = 0; i < dtdsPathsLocal->Length(); i++)
  {
    if ((dtdsPathsLocal).Has(i) && (dtdsPathsLocal).Get(i).IsString())
    {
      Napi::String value = (dtdsPathsLocal).Get(i)->ToString();
      Napi::String pathV8(env, value->ToString());
      string pathStr(*pathV8);
      const char *path(*pathV8);
      xmlChar *pathDTDCasted = xmlCharStrdup(path);
      xmlDtdPtr dtd = xmlParseDTD(NULL, pathDTDCasted);
      if (dtd == NULL)
      {
        //DTD is bad, we set error and not assign it + next
        errors.Set(i, Napi::String::New(env, pathStr.c_str()));
        continue;
      }
      libxml->dtdsPaths.push_back(dtd);
    }
  }
  xmlSetStructuredErrorFunc(NULL, NULL);
  // We set dtdsLoadedErrors property for js side
  if (errors->Length())
  {
    info.Holder().Set(Napi::String::New(env, "dtdsLoadedErrors"), errors);
  }
}

// Napi::Value Libxml::loadDtdsFromString(const Napi::CallbackInfo& info){
//   if (info.Length() < 1){
//     Napi::TypeError::New(env, "loadDtds requires at least 1 argument, an array of DTDs").ThrowAsJavaScriptException();
return env.Null();
//   }
//   if(!info[0].IsArray()){
//     Napi::TypeError::New(env, "loadDtds requires an array").ThrowAsJavaScriptException();
return env.Null();
//   }
//   Napi::EscapableHandleScope scope(env);
//   Libxml* libxml = info.Holder().Unwrap<Libxml>();
//   Napi::Array dtdsStrLocal = info[0].As<Napi::Array>();
//   Napi::Array errors = Napi::Array::New(env);
//   int options;
//   options = (XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET);
//   // disbale errors erreurs etc
//   //Napi::Array errorsDTD = Napi::Array::New(env);
//   xmlResetLastError();
//   xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
//             XmlSyntaxError::PushToArray);
//   for (unsigned int i = 0; i < dtdsStrLocal->Length(); i++){
//     if ((dtdsStrLocal).Has(i) && (dtdsStrLocal).Get(i).IsString()) {
//       Napi::String value = (dtdsStrLocal).Get(i)->ToString();
//       Napi::String strV8(env, value->ToString());
//       string pathStr (*strV8);
//       xmlDocPtr dtdDoc = xmlReadMemory(pathStr.c_str(),pathStr.Length(),NULL,NULL,options);;
//       if (dtdDoc == NULL) {
//         //DTD is bad, we set error and not assign it + next
//         errors.Set(i, Napi::String::New(env, pathStr.c_str()));
//         continue;
//       }
//       // Parse DTD from memory
//       xmlParserInputBufferPtr dtdBuf = xmlParserInputBufferCreateMem(pathStr.c_str(), pathStr.size(),
//                                                                XML_CHAR_ENCODING_UTF8);
//       if (!dtdBuf) {
//         errors.Set(i, Napi::String::New(env, pathStr.c_str()));
//         continue;
//       }
//       xmlDtdPtr pDtd = xmlIOParseDTD(NULL, dtdBuf, XML_CHAR_ENCODING_UTF8);
//       if (pDtd == NULL) {
//         xmlFreeDtd(pDtd);
//         errors.Set(i, Napi::String::New(env, pathStr.c_str()));
//         continue;
//       }
//       libxml->dtdsPaths.push_back(pDtd);
//     }
//   }
//   xmlSetStructuredErrorFunc(NULL, NULL);
//   // We set dtdsLoadedErrors property for js side
//   if(errors->Length()){
//     info.Holder().Set(Napi::String::New(env, "dtdsLoadedErrors"), errors);
//   }
// }

Napi::Value Libxml::loadSchemas(const Napi::CallbackInfo &info)
{
  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "loadSchemas requires at least 1 argument, an array of Schemas").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!info[0].IsArray())
  {
    Napi::TypeError::New(env, "loadSchemas requires an array").ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::EscapableHandleScope scope(env);
  Libxml *libxml = info.Holder().Unwrap<Libxml>();
  Napi::Array schemasPathsLocal = info[0].As<Napi::Array>();
  Napi::Array errors = Napi::Array::New(env);
  // disbale errors erreurs etc
  //Napi::Array errorsDTD = Napi::Array::New(env);
  xmlResetLastError();
  xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
                            XmlSyntaxError::PushToArray);
  for (unsigned int i = 0; i < schemasPathsLocal->Length(); i++)
  {
    if ((schemasPathsLocal).Has(i) && (schemasPathsLocal).Get(i).IsString())
    {
      Napi::String value = (schemasPathsLocal).Get(i)->ToString();
      Napi::String pathV8(env, value->ToString());
      string pathStr(*pathV8);
      const char *path(*pathV8);
      xmlSchemaParserCtxtPtr pctxt;
      xmlSchemaPtr schema;
      // If cannot create Parse schema, just continue
      if ((pctxt = xmlSchemaNewParserCtxt(path)) == NULL)
      {
        errors.Set(i, Napi::String::New(env, pathStr.c_str()));
        continue;
      }
      // Chargement du contenu du XML Schema
      schema = xmlSchemaParse(pctxt);
      xmlSchemaFreeParserCtxt(pctxt);
      if (schema == NULL)
      {
        errors.Set(i, Napi::String::New(env, pathStr.c_str()));
        continue;
      }
      libxml->schemasPaths.push_back(schema);
    }
  }
  xmlSetStructuredErrorFunc(NULL, NULL);
  // We set dtdLoadedErrors property for js side
  if (errors->Length())
  {
    info.Holder().Set(Napi::String::New(env, "schemasLoadedErrors"), errors);
  }
}

Napi::Value Libxml::validateAgainstDtds(const Napi::CallbackInfo &info)
{
  Libxml *libxml = info.Holder().Unwrap<Libxml>();

  if (libxml->dtdsPaths.empty())
  {
    return env.Null();
    return;
  }

  if (int newMaxNbError = info[0].As<Napi::Number>().Int64Value())
  {
    XmlSyntaxError::ChangeMaxNumberOfError(newMaxNbError);
  }

  //Setting contexte of validation
  const char *dtdValidationErrorsPath;
  bool oneOfTheDtdValidate = false;
  string dtdValidateName;

  //If length 0, return null; to implement
  Napi::Object errorsValidations = Napi::Object::New(env);

  for (vector<xmlDtdPtr>::iterator dtd = libxml->dtdsPaths.begin(); dtd != libxml->dtdsPaths.end(); ++dtd)
  {
    const char *dtdName = (const char *)(*dtd)->SystemID;
    //Napi::Array errors = New<v8::Array>(3);
    Napi::Array errors = Napi::Array::New(env);
    xmlResetLastError();
    xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
                              XmlSyntaxError::PushToArray);
    Napi::String SystemIDString = Napi::String::New(env, dtdName);
    // Création du contexte de validation
    xmlValidCtxtPtr vctxt;
    //vctxt = xmlNewValidCtxt();
    if ((vctxt = xmlNewValidCtxt()) == NULL)
    {
      continue;
    }
    //Instead we could set this to disable output : xmlSetStructuredErrorFunc(vctxt,errorsHandler);
    vctxt->userData = (void *)Libxml::errorsHandler;
    vctxt->error = (xmlValidityErrorFunc)Libxml::errorsHandler;
    vctxt->warning = (xmlValidityWarningFunc)Libxml::errorsHandler;
    // Validation
    int result = xmlValidateDtd(vctxt, libxml->docPtr, *dtd);
    // Libération des erreurs etc
    xmlSetStructuredErrorFunc(NULL, NULL);
    xmlFreeValidCtxt(vctxt);
    if (result != 0)
    {
      oneOfTheDtdValidate = true;
      dtdValidateName = dtdName;
      break;
    }
    errorsValidations.Set(SystemIDString, errors);
    dtdValidationErrorsPath = dtdName;
  }
  if (oneOfTheDtdValidate)
  {
    if (dtdValidateName.Length())
    {
      return Napi::String::New(env, dtdValidateName);
    }
    else
    {
      return env.True();
    }
  }
  else
  {
    info.Holder().Set(Napi::String::New(env, "validationDtdErrors"), errorsValidations);
    return env.False();
  }
}

Napi::Value Libxml::validateAgainstSchemas(const Napi::CallbackInfo &info)
{
  Libxml *libxml = info.Holder().Unwrap<Libxml>();

  if (libxml->schemasPaths.empty())
  {
    return env.Null();
    return;
  }

  if (int newMaxNbError = info[0].As<Napi::Number>().Int64Value())
  {
    XmlSyntaxError::ChangeMaxNumberOfError(newMaxNbError);
  }
  //Setting contexte of validation
  const char *schemaValidationErrorsPath;
  bool oneOfTheSchemaValidate = false;
  string schemaValidateName;

  //If length 0, return null; to implement
  Napi::Object errorsValidations = Napi::Object::New(env);

  for (vector<xmlSchemaPtr>::iterator xsd = libxml->schemasPaths.begin(); xsd != libxml->schemasPaths.end(); ++xsd)
  {
    Napi::Array errors = Napi::Array::New(env);
    xmlResetLastError();
    xmlSetStructuredErrorFunc(reinterpret_cast<void *>(&errors),
                              XmlSyntaxError::PushToArray);
    const char *xsdName = (const char *)(*xsd)->doc->URL;
    Napi::String urlSchema = Napi::String::New(env, xsdName);
    // Création du contexte de validation
    xmlSchemaValidCtxtPtr vctxt;
    if ((vctxt = xmlSchemaNewValidCtxt(*xsd)) == NULL)
    {
      continue;
    }
    //Instead we could set this to disable output : xmlSetStructuredErrorFunc(vctxt,errorsHandler);
    xmlSchemaSetValidErrors(vctxt, (xmlSchemaValidityErrorFunc)Libxml::errorsHandler, (xmlSchemaValidityWarningFunc)Libxml::errorsHandler, (void *)Libxml::errorsHandler);
    int result = xmlSchemaValidateDoc(vctxt, libxml->docPtr);
    // Libération des erreurs etc
    xmlSetStructuredErrorFunc(NULL, NULL);
    xmlSchemaFreeValidCtxt(vctxt);
    if (result == 0)
    {
      oneOfTheSchemaValidate = true;
      schemaValidateName = xsdName;
      break;
    }
    errorsValidations.Set(urlSchema, errors);
    schemaValidationErrorsPath = xsdName;
  }
  if (oneOfTheSchemaValidate)
  {
    if (schemaValidateName.Length())
    {
      return Napi::String::New(env, schemaValidateName);
    }
    else
    {
      return env.True();
    }
  }
  else
  {
    info.Holder().Set(Napi::String::New(env, "validationSchemaErrors"), errorsValidations);
    return env.False();
  }
}

Napi::Value Libxml::xpathSelect(const Napi::CallbackInfo &info)
{
  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "xpathSelect requires at least 1 argument").ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::EscapableHandleScope scope(env);
  Napi::Value res;
  Libxml *libxml = info.Holder().Unwrap<Libxml>();
  Napi::String val(env, info[0].ToString());
  const char *xpathToGet(*val);
  xmlChar *xpathExpr = xmlCharStrdup(xpathToGet);
  xmlXPathContextPtr xpathCtx;
  xmlXPathObjectPtr xpathObj;
  /* Create xpath evaluation context */
  xpathCtx = xmlXPathNewContext(libxml->docPtr);
  if (xpathCtx == NULL)
  {
    Napi::TypeError::New(env, "Error: unable to create new XPath context").ThrowAsJavaScriptException();
    return env.Null();
  }
  /* Evaluate xpath expression */
  xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
  if (xpathObj == NULL)
  {
    xmlXPathFreeContext(xpathCtx);
    return env.False();
  }
  else if (xpathObj)
  {
    switch (xpathObj->type)
    {
    case XPATH_BOOLEAN:
      res = Napi::Boolean::New(env, xpathObj->boolval);
      break;
    case XPATH_NUMBER:
      res = Napi::Number::New(env, xpathObj->floatval);
      break;
    case XPATH_STRING:
      res = Napi::String::New(env, (const char *)xpathObj->stringval,
                              xmlStrlen(xpathObj->stringval));
      break;
    default:
      res = env.Null();
      break;
    }
  }
  xmlXPathFreeObject(xpathObj);
  xmlXPathFreeContext(xpathCtx);
  return return scope.Escape(res);
}

Napi::Value Libxml::freeXml(const Napi::CallbackInfo &info)
{
  Napi::HandleScope scope(env);
  Libxml *libxml = info.Holder().Unwrap<Libxml>();
  // Delete Javascript property
  bool deleted = Napi::Delete(info.Holder(), Napi::String::New(env, "wellformedErrors")).FromMaybe(false);
  // If doc is already null, just do nothing and only available on manual mod
  if (libxml->docPtr == NULL)
  {
    return;
  }
  // Force clear memory DTD loaded
  xmlFreeDoc(libxml->docPtr);
  libxml->docPtr = NULL;
};

Napi::Value Libxml::freeDtds(const Napi::CallbackInfo &info)
{
  Napi::HandleScope scope(env);
  Libxml *libxml = info.Holder().Unwrap<Libxml>();
  // Delete Javascript property
  bool deletedLoaded = Napi::Delete(info.Holder(), Napi::String::New(env, "dtdsLoadedErrors")).FromMaybe(false);
  bool deleted = Napi::Delete(info.Holder(), Napi::String::New(env, "validationDtdErrors")).FromMaybe(false);
  // If dtds is already empty, just stop here
  if (libxml->dtdsPaths.empty())
  {
    return;
  }
  for (vector<xmlDtdPtr>::iterator dtd = libxml->dtdsPaths.begin(); dtd != libxml->dtdsPaths.end(); ++dtd)
  {
    if (*dtd != NULL)
    {
      // Force clear memory DTD loaded
      xmlFreeDtd(*dtd);
      *dtd = nullptr;
    }
  }
  // clear the vector of dtds
  libxml->dtdsPaths.clear();
}

Napi::Value Libxml::freeSchemas(const Napi::CallbackInfo &info)
{
  Napi::HandleScope scope(env);
  Libxml *libxml = info.Holder().Unwrap<Libxml>();
  // Delete Javascript property
  bool deletedLoaded = Napi::Delete(info.Holder(), Napi::String::New(env, "schemasLoadedErrors")).FromMaybe(false);
  bool deleted = Napi::Delete(info.Holder(), Napi::String::New(env, "validationSchemasErrors")).FromMaybe(false);
  // If dtds is already empty, just stop here
  if (libxml->schemasPaths.empty())
  {
    return;
  }
  for (vector<xmlSchemaPtr>::iterator xsd = libxml->schemasPaths.begin(); xsd != libxml->schemasPaths.end(); ++xsd)
  {
    if (*xsd != NULL)
    {
      // Force clear memory xsd loaded
      xmlSchemaFree(*xsd);
      *xsd = nullptr;
    }
  }
  // clear the vector of dtds
  libxml->schemasPaths.clear();
}

Napi::Value Libxml::clearAll(const Napi::CallbackInfo &info)
{
  Napi::HandleScope scope(env);
  Libxml *libxml = info.Holder().Unwrap<Libxml>();
  // Attention this function clear all the libxml2 memory for all threads!
  xmlCleanupParser();
}

void InitLibxml(Napi::Object exports) { Libxml::Init(exports); }

NODE_API_MODULE(libxml, InitLibxml);
