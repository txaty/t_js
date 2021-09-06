#include <math.h>
#include "js_stringnify.h"
#include "js_include.h"

// Stringify string JS value
size_t strstring(JS *js, jsval_t value, char *buf, size_t len)
{
  jsoff_t n, off = vstr(js, value, &n);
  // printf("STRING: len %u, off %lu %zu\n", n, off - sizeof(off), len);
  return snprintf(buf, len, "\"%.*s\"", (int)n, (char *)js->mem + off);
}

// Stringify JS object
size_t strobj(JS *js, jsval_t obj, char *buf, size_t len)
{
  size_t n = snprintf(buf, len, "%s", "{");
  jsoff_t next = loadoff(js, vdata(obj)) & ~3; // Load first prop offset
  while (next < js->brk && next != 0)
  { // Iterate over props
    jsoff_t koff = loadoff(js, next + sizeof(next));
    jsval_t val = loadval(js, next + sizeof(next) + sizeof(koff));
    // printf("PROP %u, koff %u\n", next & ~3, koff);
    n += snprintf(buf + n, len - n, "%s", n == 1 ? "" : ",");
    n += tostr(js, mkval(JS_STR, koff), buf + n, len - n);
    n += snprintf(buf + n, len - n, "%s", ":");
    n += tostr(js, val, buf + n, len - n);
    next = loadoff(js, next) & ~3; // Load next prop offset
  }
  return n + snprintf(buf + n, len - n, "%s", "}");
}

// Stringify numeric JS value
size_t strnum(jsval_t value, char *buf, size_t len)
{
  double dv = tod(value), iv;
  if (modf(dv, &iv) == 0.0)
    return snprintf(buf, len, "%d", (int)dv);
  int int_p, fract_p0, fract_p1;
  int_p = (int)dv;
  fract_p0 = (int)((dv - int_p) * 10) % 10;
  fract_p1 = (int)((dv - int_p) * 100) % 10;
  if (fract_p1 == 0)
    return snprintf(buf, len, "%d.%d", int_p, fract_p0);
  int fract_p2 = (int)((dv - int_p) * 1000) % 10;
  if (fract_p2 == 0)
    return snprintf(buf, len, "%d.%d%d", int_p, fract_p0, fract_p1);
  int fract_p3 = (int)((dv - int_p) * 10000) % 10;
  if (fract_p3 == 0)
    return snprintf(buf, len, "%d.%d%d%d", int_p, fract_p0, fract_p1, fract_p2);
  int fract_p4 = (int)((dv - int_p) * 100000) % 10;
  if (fract_p4 == 0)
    return snprintf(buf, len, "%d.%d%d%d%d", int_p, fract_p0, fract_p1, fract_p2, fract_p3);
  return snprintf(buf, len, "%d.%d%d%d%d%d", int_p, fract_p0, fract_p1, fract_p2, fract_p3, fract_p4);
}

// Stringify JS function
size_t strfunc(JS *js, jsval_t value, char *buf, size_t len)
{
  jsoff_t n, off = vstr(js, value, &n), isjs = (js->mem[off] == '(');
  char *p = (char *)&js->mem[off];
  return isjs ? snprintf(buf, len, "function%.*s", (int)n, p) // JS function
              : snprintf(buf, len, "\"%.*s\"", (int)n, p);    // C function
}

// Stringify JS value into the given buffer
size_t tostr(JS *js, jsval_t value, char *buf, size_t len)
{
  switch (vtype(value))
  {
  case JS_UNDEF:
    return snprintf(buf, len, "%s", "undefined");
  case JS_NULL:
    return snprintf(buf, len, "%s", "null");
  case JS_BOOL:
    return snprintf(buf, len, "%s", vdata(value) & 1 ? "true" : "false");
  case JS_OBJ:
    return strobj(js, value, buf, len);
  case JS_STR:
    return strstring(js, value, buf, len);
  case JS_NUM:
    return strnum(value, buf, len);
  case JS_FUNC:
    return strfunc(js, value, buf, len);
  default:
    return snprintf(buf, len, "VTYPE%d", vtype(value));
  }
}

