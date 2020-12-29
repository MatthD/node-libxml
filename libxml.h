#ifndef LIBXML_H
#define LIBXML_H

#include "napi.h"
#include <string>
#include <vector>

#include "libxml/parser.h"
#include "libxml/valid.h"
#include "libxml/xmlschemastypes.h"
#include "libxml/xmlschemas.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"

#include "libxml-syntax-error.h"

enum
{
  ERROR_OCCURED = -1, // Une erreur est survenue pendant la validation
  NOT_VALID = 0,      // Le document n'est pas valide
  VALID = 1           // Le document est valide
};

using namespace std;

class Libxml : public Napi::ObjectWrap<Libxml>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  Libxml(const Napi::CallbackInfo &info);

private:
  // static void errorsHandler(void *, xmlErrorPtr);
  // explicit Libxml();
  xmlDocPtr docPtr = NULL;
  string path;
  vector<xmlDtdPtr> dtdsPaths;
  vector<xmlSchemaPtr> schemasPaths;
  static Napi::FunctionReference constructor;
  Napi::Value loadXml(const Napi::CallbackInfo &info);
  Napi::Value loadXmlFromString(const Napi::CallbackInfo& info);
  Napi::Value loadDtds(const Napi::CallbackInfo& info);
  // // static Napi::Value loadDtdsFromString(const Napi::CallbackInfo& info);
  // static Napi::Value loadSchemas(const Napi::CallbackInfo& info);
  Napi::Value validateAgainstDtds(const Napi::CallbackInfo& info);
  // static Napi::Value validateAgainstSchemas(const Napi::CallbackInfo& info);
  // static Napi::Value xpathSelect(const Napi::CallbackInfo& info);
  Napi::Value getDtd(const Napi::CallbackInfo& info);
  void freeXml(const Napi::CallbackInfo& info);
  Napi::Value freeDtds(const Napi::CallbackInfo& info);
  // static Napi::Value freeSchemas(const Napi::CallbackInfo& info);
  // static Napi::Value clearAll(const Napi::CallbackInfo& info);
};

#endif
