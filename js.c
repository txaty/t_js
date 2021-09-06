#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "js.h"
#include "js_include.h"

char JS_MEM[JS_CFG_MEM_SIZE];

JS *js_create(void *buf, size_t len)
{
  JS *js = NULL;
  if (len < sizeof(*js) + esize(JS_OBJ))
    return js;
  memset(buf, 0, len);                     // Important!
  js = (JS *)buf;                          // JS lives at the beginning
  js->mem = (uint8_t *)(js + 1);           // Then goes memory for JS data
  js->size = (jsoff_t)(len - sizeof(*js)); // JS memory size
  js->scope = mkobj(js, 0);                // Create global scope
  return js;
}

JS* js_create_static()
{
  return js_create(JS_MEM, sizeof(JS_MEM));
}

// Stringify JS value into a free JS memory block
const char *js_str(JS *js, jsval_t value)
{
  // Leave jsoff_t placeholder between js->brk and a stringify buffer,
  // in case if next step is convert it into a JS variable
  char *buf = (char *)&js->mem[js->brk + sizeof(jsoff_t)];
  if (is_err(value))
    return js->errmsg;
  if (js->brk + sizeof(jsoff_t) >= js->size)
    return "";
  tostr(js, value, buf, js->size - js->brk - sizeof(jsoff_t));
  // printf("JSSTR: %d [%s]\n", vtype(value), buf);
  return buf;
}

jsval_t js_mkobj(JS *js)
{
  return mkobj(js, 0);
}

jsval_t js_glob(JS *js)
{
  (void)js;
  return mkval(JS_OBJ, 0);
}

void js_set(JS *js, jsval_t obj, const char *key, jsval_t val)
{
  is_err(setprop(js, obj, mkstr(js, key, strlen(key)), val));
}

int js_usage(JS *js)
{
  return js->brk * 100 / js->size;
}

jsval_t js_import(JS *js, uintptr_t fn, const char *signature)
{
  char buf[64];
  size_t n = snprintf(buf, sizeof(buf), "%s@%" PRIxPTR, signature, fn);
  jsval_t str = mkstr(js, buf, n);
  return mkval(JS_FUNC, vdata(str));
}

jsval_t js_eval(JS *js, const char *buf, size_t len)
{
  // printf("EVAL: [%.*s]\n", (int) len, buf);
  if (len == (size_t)~0)
    len = strlen(buf);
  return js_eval_nogc(js, buf, (jsoff_t)len);
}

jsval_t js_execute_script(JS* js, const char *filename)
{
    char data[32 * 1024];
    FILE *fp = fopen(filename, "rb");
    if (!fp)
        return 0;
    size_t len = fread(data, 1, sizeof(data), fp);
    return js_eval(js, data, len);
}