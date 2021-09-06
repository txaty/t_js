#include <string.h>
#include "js_entity.h"
#include "js_include.h"

jsval_t mkentity(JS *js, jsoff_t b, const void *buf, size_t len)
{
  jsoff_t ofs = js_alloc(js, len + sizeof(b));
  if (ofs == (jsoff_t)~0)
    return js_err(js, "oom");
  memcpy(&js->mem[ofs], &b, sizeof(b));
  // Using memmove - in case we're stringifying data from the free JS mem
  if (buf != NULL)
    memmove(&js->mem[ofs + sizeof(b)], buf, len);
  if ((b & 3) == JS_STR)
    js->mem[ofs + sizeof(b) + len] = 0; // 0-terminate
  // printf("MKE: %u @ %u type %d\n", js->brk - ofs, ofs, b & 3);
  return mkval(b & 3, ofs);
}

// Return JS_OBJ/JS_PROP/JS_STR entity size based on the first word in memory
inline jsoff_t esize(jsoff_t w)
{
  switch (w & 3)
  {
  case JS_OBJ:
    return sizeof(jsoff_t) + sizeof(jsoff_t);
  case JS_PROP:
    return sizeof(jsoff_t) + sizeof(jsoff_t) + sizeof(jsval_t);
  case JS_STR:
    return sizeof(jsoff_t) + align32(w >> 2);
  default:
    return (jsoff_t)~0;
  }
}