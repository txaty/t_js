#include "js_basic.h"
#include "js_include.h"

uint8_t unhex(uint8_t c)
{
  return (c >= '0' && c <= '9') ? c - '0' : (c >= 'a' && c <= 'f') ? c - 'W'
                                        : (c >= 'A' && c <= 'F')   ? c - '7'
                                                                   : 0;
}

uint64_t unhexn(const uint8_t *s, int len)
{
  uint64_t v = 0;
  for (int i = 0; i < len; i++)
  {
    if (i > 0)
      v <<= 4;
    v |= unhex(s[i]);
  }
  return v;
}

jsoff_t offtolen(jsoff_t off)
{
  return (off >> 2) - 1;
}

jsoff_t vstrlen(JS *js, jsval_t v)
{
  return offtolen(loadoff(js, vdata(v)));
}

const char *typestr(uint8_t t)
{
  const char *names[] = {"object", "prop", "string", "undefined", "null",
                         "number", "boolean", "function", "nan"};
  return (t < sizeof(names) / sizeof(names[0])) ? names[t] : "??";
}

jsval_t upper(JS *js, jsval_t scope)
{
  return mkval(JS_OBJ, loadoff(js, vdata(scope) + sizeof(jsoff_t)));
}

jsoff_t align32(jsoff_t v)
{
  return ((v + 3) >> 2) << 2;
}

// Return mem offset and length of the JS string
jsoff_t vstr(JS *js, jsval_t value, jsoff_t *len)
{
  jsoff_t off = vdata(value);
  if (len)
    *len = offtolen(loadoff(js, off));
  return off + sizeof(off);
}