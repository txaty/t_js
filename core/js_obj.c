#include "js_obj.h"
#include "js_include.h"

jsval_t mkobj(JS *js, jsoff_t parent)
{
  return mkentity(js, 0 | JS_OBJ, &parent, sizeof(parent));
}

// Seach for property in a single object
jsoff_t lkp(JS *js, jsval_t obj, const char *buf, size_t len)
{
  jsoff_t off = loadoff(js, vdata(obj)) & ~3; // Load first prop offset
  // printf("LKP: %lu %u [%.*s]\n", vdata(obj), off, (int) len, buf);
  while (off < js->brk && off != 0)
  { // Iterate over props
    jsoff_t koff = loadoff(js, off + sizeof(off));
    jsoff_t klen = (loadoff(js, koff) >> 2) - 1;
    const char *p = (char *)&js->mem[koff + sizeof(koff)];
    // printf("  %u %u[%.*s]\n", off, (int) klen, (int) klen, p);
    if (streq(buf, len, p, klen))
      return off;                // Found !
    off = loadoff(js, off) & ~3; // Load next prop offset
  }
  return 0; // Not found
}