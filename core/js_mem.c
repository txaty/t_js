#include <assert.h>
#include <string.h>
#include "js_mem.h"
#include "js_include.h"

void saveoff(JS *js, jsoff_t off, jsoff_t val)
{
    memcpy(&js->mem[off], &val, sizeof(val));
}

void saveval(JS *js, jsoff_t off, jsval_t val)
{
    memcpy(&js->mem[off], &val, sizeof(val));
}

jsval_t loadval(JS *js, jsoff_t off)
{
  jsval_t v = 0;
  memcpy(&v, &js->mem[off], sizeof(v));
  return v;
}

jsoff_t loadoff(JS *js, jsoff_t off)
{
    jsoff_t v = 0;
    assert(js->brk <= js->size);
    memcpy(&v, &js->mem[off], sizeof(v));
    return v;
}

jsoff_t js_alloc(JS *js, size_t size)
{
    jsoff_t ofs = js->brk;
    size = align32((jsoff_t)size); // 4-byte align, (n + k - 1) / k * k
    if (js->brk + size > js->size)
        return ~(jsoff_t)0;
    js->brk += (jsoff_t)size;
    return ofs;
}

bool is_mem_entity(uint8_t t)
{
    return t == JS_OBJ || t == JS_PROP || t == JS_STR || t == JS_FUNC;
}

void js_fixup_offsets(JS *js, jsoff_t start, jsoff_t size)
{
    for (jsoff_t n, v, off = 0; off < js->brk; off += n)
    { // start from 0!
        v = loadoff(js, off);
        n = esize(v & ~MARK);
        if (v & MARK)
            continue; // To be deleted, don't bother
        if ((v & 3) != JS_OBJ && (v & 3) != JS_PROP)
            continue;
        if (v > start)
            saveoff(js, off, v - size);
        if ((v & 3) == JS_PROP)
        {
            jsoff_t koff = loadoff(js, off + sizeof(off));
            if (koff > start)
                saveoff(js, off + sizeof(off), koff - size);
            jsval_t val = loadval(js, off + sizeof(off) + sizeof(off));
            if (is_mem_entity(vtype(val)) && vdata(val) > start)
            {
                // printf("MV %u %lu -> %lu\n", off, vdata(val), vdata(val) - size);
                saveval(js, off + sizeof(off) + sizeof(off),
                        mkval(vtype(val), vdata(val) - size));
            }
        }
    }
    for (jsoff_t i = 0; i < js->ncbs; i++)
    {
        jsoff_t base = js->size + i * 3 * sizeof(jsoff_t) + sizeof(jsoff_t);
        jsoff_t o1 = loadoff(js, base), o2 = loadoff(js, base + sizeof(o1));
        if (o1 > start)
            saveoff(js, base, o1 - size);
        if (o2 > start)
            saveoff(js, base + sizeof(jsoff_t), o2 - size);
    }
    // Fixup js->scope
    jsoff_t off = vdata(js->scope);
    if (off > start)
        js->scope = mkval(JS_OBJ, off - size);
}

uint8_t getri(uint32_t mask, uint8_t ri)
{
  while (ri > 0 && (mask & (1 << ri)))
    ri--;
  if (!(mask & (1 << ri)))
    ri++;
  return ri;
}