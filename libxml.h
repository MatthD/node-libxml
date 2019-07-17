#ifndef LIBXML_H
#define LIBXML_H

#include <nan.h>
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

enum {
  ERROR_OCCURED = -1, // Une erreur est survenue pendant la validation
  NOT_VALID = 0,      // Le document n'est pas valide
  VALID = 1           // Le document est valide
};

using namespace std;

class Libxml : public Nan::ObjectWrap {
public:

  ~Libxml();

  static NAN_MODULE_INIT(Init);
  xmlDocPtr docPtr = NULL;
  vector<xmlDtdPtr> dtdsPaths;
  vector<xmlSchemaPtr> schemasPaths;

private:
  static const char* ToChars(v8::Local<v8::Value> val);
  static void errorsHandler(void *, xmlErrorPtr);
  explicit Libxml();
  static inline Nan::Persistent<v8::Function>& constructor();

  static NAN_METHOD(New);
  static NAN_METHOD(loadXml);
  static NAN_METHOD(loadXmlFromString);
  static NAN_METHOD(loadDtds);
  // static NAN_METHOD(loadDtdsFromString);
  static NAN_METHOD(loadSchemas);
  static NAN_METHOD(validateAgainstDtds);
  static NAN_METHOD(validateAgainstSchemas);
  static NAN_METHOD(xpathSelect);
  static NAN_METHOD(getDtd);
  static NAN_METHOD(freeXml);
  static NAN_METHOD(freeDtds);
  static NAN_METHOD(freeSchemas);
  static NAN_METHOD(clearAll);

};

#endif
