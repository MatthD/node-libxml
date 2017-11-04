#ifndef LIBXML_SYNTAX_ERROR
#define LIBXML_SYNTAX_ERROR

#include <libxml/xmlerror.h>

#include "libxml.h"

class XmlSyntaxError {
public:
  // push xmlError onto v8::Array
  static void PushToArray(void* errs, xmlError* error);

  // create a v8 object for the syntax eror
  static v8::Local<v8::Value> BuildSyntaxError(xmlError* error);
};

// LIBXML_SYNTAX_ERROR
#endif  
