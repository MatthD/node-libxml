#ifndef LIBXML_H
#define LIBXML_H

#include <nan.h>

#include "libxml/parser.h"
#include "libxml/valid.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"

#include "libxml-syntax-error.h"

enum {
  ERROR_OCCURED = -1, // Une erreur est survenue pendant la validation
  NOT_VALID = 0,      // Le document n'est pas valide
  VALID = 1           // Le document est valide
};

class Libxml : public Nan::ObjectWrap {
public:

  virtual ~Libxml();

  static NAN_MODULE_INIT(Init);
  bool manual = false;
  xmlDocPtr docPtr;
  xmlDtdPtr dtdPtr;
  // Libxml::document* doc;

private:
  static void errorsHandler(void *, xmlErrorPtr);
  explicit Libxml(bool manual = false); 
  static inline Nan::Persistent<v8::Function>& constructor();

  static NAN_METHOD(New);
  static NAN_METHOD(loadXml);
  static NAN_METHOD(loadDtd);
  static NAN_METHOD(xpathSelect);
  static NAN_METHOD(getDtd);
  static NAN_METHOD(validateAgainstDtd);
  static NAN_METHOD(freeXml);
  static NAN_METHOD(clearAll);
  
};

#endif
