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

  // push xmlError onto Napi::Array
  static void PushToArray(void *errs, xmlError *error);
  static void PushToArray(Napi::Array& errs, const char* errorMessage);

  // create a Napi object for the syntax eror
  static Napi::Value BuildSyntaxError(xmlError *error, Napi::Env env);
};

// LIBXML_SYNTAX_ERROR
#endif
