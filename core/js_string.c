#include <string.h>
#include "js_string.h"
#include "js_include.h"

jsval_t mkstr(JS *js, const void *ptr, size_t len)
{
  // printf("MKSTR: [%.*s] -> off %u\n", (int) len, (char *) ptr, js->brk);
  return mkentity(js, (jsoff_t)(((len + 1) << 2) | JS_STR), ptr, len + 1);
}

bool streq(const char *buf, size_t len, const char *p, size_t n)
{
  return n == len && memcmp(buf, p, len) == 0;
}