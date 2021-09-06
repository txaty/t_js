#include <string.h>
#include "js_prop.h"
#include "js_include.h"

jsval_t setprop(JS *js, jsval_t obj, jsval_t k, jsval_t v)
{
  jsoff_t koff = vdata(k);                   // Key offset
  jsoff_t b, head = vdata(obj);              // Property list head
  char buf[sizeof(koff) + sizeof(v)];        // Property memory layout
  memcpy(&b, &js->mem[head], sizeof(b));     // Load current 1st prop offset
  memcpy(buf, &koff, sizeof(koff));          // Initialize prop data: copy key
  memcpy(buf + sizeof(koff), &v, sizeof(v)); // Copy value
  jsoff_t brk = js->brk | JS_OBJ;            // New prop offset
  memcpy(&js->mem[head], &brk, sizeof(brk)); // Repoint head to the new prop
  // printf("PROP: %u -> %u\n", b, brk);
  return mkentity(js, (b & ~3) | JS_PROP, buf, sizeof(buf)); // Create new prop
}

jsval_t resolveprop(JS *js, jsval_t v)
{
  if (vtype(v) != JS_PROP)
    return v;
  v = loadval(js, vdata(v) + sizeof(jsoff_t) + sizeof(jsoff_t));
  return resolveprop(js, v);
}