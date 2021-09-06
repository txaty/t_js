#include <string.h>
#include "js_gc.h"
#include "js_include.h"

void js_delete_marked_entities(JS *js)
{
  for (jsoff_t n, v, off = 0; off < js->brk; off += n)
  {
    v = loadoff(js, off);
    n = esize(v & ~MARK);
    if (v & MARK)
    { // This entity is marked for deletion, remove it
      // printf("DEL: %4u %d %x\n", off, v & 3, n);
      // assert(off + n <= js->brk);
      js_fixup_offsets(js, off, n);
      memmove(&js->mem[off], &js->mem[off + n], js->brk - off - n);
      js->brk -= n; // Shrink brk boundary by the size of deleted entity
      n = 0;        // We shifted data, next iteration stay on this offset
    }
  }
}

void js_mark_all_entities_for_deletion(JS *js)
{
  for (jsoff_t v, off = 0; off < js->brk; off += esize(v))
  {
    v = loadoff(js, off);
    saveoff(js, off, v | MARK);
  }
}

static jsoff_t js_unmark_entity(JS *js, jsoff_t off)
{
  jsoff_t v = loadoff(js, off);
  if (v & MARK)
  {
    saveoff(js, off, v & ~MARK);
    // printf("UNMARK %5u\n", off);
    if ((v & 3) == JS_OBJ)
      js_unmark_entity(js, v & ~(MARK | 3));
    if ((v & 3) == JS_PROP)
    {
      js_unmark_entity(js, v & ~(MARK | 3));                // Unmark next prop
      js_unmark_entity(js, loadoff(js, off + sizeof(off))); // Unmark key
      jsval_t val = loadval(js, off + sizeof(off) + sizeof(off));
      if (is_mem_entity(vtype(val)))
        js_unmark_entity(js, vdata(val));
    }
  }
  return v & ~(MARK | 3);
}

void js_unmark_used_entities(JS *js)
{
  for (jsval_t scope = js->scope;;)
  {
    js_unmark_entity(js, vdata(scope));
    jsoff_t off = loadoff(js, vdata(scope)) & ~3;
    while (off < js->brk && off != 0)
      off = js_unmark_entity(js, off);
    if (vdata(scope) == 0)
      break; // Last (global) scope processed
    scope = upper(js, scope);
  }
  for (jsoff_t i = 0; i < js->ncbs; i++)
  {
    jsoff_t base = js->size + i * 3 * sizeof(jsoff_t) + sizeof(jsoff_t);
    js_unmark_entity(js, loadoff(js, base));
    js_unmark_entity(js, loadoff(js, base + sizeof(jsoff_t)));
  }
}

void js_gc(JS *js)
{
  // printf("GC RUN\n");
  js_mark_all_entities_for_deletion(js);
  js_unmark_used_entities(js);
  js_delete_marked_entities(js);
}