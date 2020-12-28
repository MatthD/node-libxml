#ifndef LIBXML_SYNTAX_ERROR
#define LIBXML_SYNTAX_ERROR

#include <libxml/xmlerror.h>

#include "libxml.h"

class XmlSyntaxError
{
  static uint32_t maxError;

public:
  static Napi::Env* env;
  static void ChangeMaxNumberOfError(int max);

  // push xmlError onto v8::Array
  static void PushToArray(void *errs, xmlError *error);

  // create a v8 object for the syntax eror
  static Napi::Value BuildSyntaxError(xmlError *error, Napi::Env env);
};

// LIBXML_SYNTAX_ERROR
#endif
