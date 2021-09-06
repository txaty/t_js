#include <stdlib.h>
#include <string.h>
#include "js_types.h"
#include "js_debug.h"

#ifdef JS_CFG_DEBUG
void js_dump(struct js *js)
{
  jsoff_t off = 0, v;
  printf("JS size %u, brk %u, callbacks: %u\n", js->size, js->brk, js->ncbs);
  while (off < js->brk)
  {
    memcpy(&v, &js->mem[off], sizeof(v));
    printf(" %5u: ", off);
    if ((v & 3) == JS_OBJ)
    {
      printf("OBJ %u %u\n", v & ~3, loadoff(js, off + sizeof(off)));
    }
    else if ((v & 3) == JS_PROP)
    {
      jsoff_t koff = loadoff(js, off + sizeof(v));
      jsval_t val = loadval(js, off + sizeof(v) + sizeof(v));
      printf("PROP next %u, koff %u vtype %d vdata %lu\n", v & ~3, koff,
             vtype(val), vdata(val));
    }
    else if ((v & 3) == JS_STR)
    {
      jsoff_t len = offtolen(v);
      printf("STR %u [%.*s]\n", len, (int)len, js->mem + off + sizeof(v));
    }
    else
    {
      printf("???\n");
      break;
    }
    off += esize(v);
  }

  for (jsoff_t i = 0; i < js->ncbs; i++)
  {
    jsoff_t base = js->size + i * 3 * sizeof(jsoff_t) + sizeof(jsoff_t);
    jsoff_t o1 = loadoff(js, base), o2 = loadoff(js, base + sizeof(o1));
    printf("FFICB %u %u\n", o1, o2);
  }
}
#endif