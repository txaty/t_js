#include <stdio.h>
#include <stdarg.h>
#include "js_err.h"
#include "js_include.h"

jsval_t js_err(JS *js, const char *fmt, ...)
{
  va_list ap;
  size_t n = snprintf(js->errmsg, sizeof(js->errmsg), "%s", "ERROR: ");
  va_start(ap, fmt);
  vsnprintf(js->errmsg + n, sizeof(js->errmsg) - n, fmt, ap);
  va_end(ap);
  js->errmsg[sizeof(js->errmsg) - 1] = '\0';
  // printf("ERR: [%s]\n", js->errmsg);
  js->pos = js->clen; // We're done.. Jump to the end of code
  js->tok = JS_TK_EOF;
  return mkval(JS_ERR, 0);
}