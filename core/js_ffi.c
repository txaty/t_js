//  C  FFI implementation start
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "js_ffi.h"
#include "js_include.h"

//struct fficbparam { JS *js; const char *decl; jsval_t jsfunc; };

#ifndef JS_NOCB
jw_t fficb(uintptr_t param, union ffi_val *args)
{
  jsoff_t size;
  memcpy(&size, (char *)param, sizeof(size));
  JS *js = (JS *)((char *)param - size) - 1;
  //printf("FFICB: js %p, param %p %u\n", js, (void *) param, size);
  jsoff_t f1len, f1off = vstr(js, loadoff(js, size + sizeof(jsoff_t)), &f1len);
  jsoff_t f2len, f2off = vstr(js, loadoff(js, size + 2 * sizeof(jsoff_t)), &f2len);
  char buf[100], *decl = (char *)&js->mem[f1off];
  jsoff_t n = 0, max = (jsoff_t)sizeof(buf);
  //printf("F1: %u[%.*s]\n", f1len, (int) f1len, decl);
  //printf("F2: %u[%.*s]\n", f2len, (int) f2len, &js->mem[f2off]);
  while (f1len > 1 && decl[0] != '[')
    decl++, f1len--;
  for (jsoff_t i = 0; i + 2 < f1len && decl[i + 2] != ']'; i++)
  {
    if (n > 0)
      n += snprintf(buf + n, max - n, "%s", ",");
    switch (decl[i + 2])
    {
    case 's':
      n += snprintf(buf + n, max - n, "'%s'", (char *)(uintptr_t)args[i].w);
      break;
    case 'i':
      n += snprintf(buf + n, max - n, "%ld", (long)args[i].w);
      break;
    case 'd':
      n += snprintf(buf + n, max - n, "%g", args[i].d);
      break;
    default:
      n += snprintf(buf + n, max - n, "%s", "null");
      break;
    }
  }
  const char *code = js->code;            // Save current parser state
  jsoff_t clen = js->clen, pos = js->pos; // code, position and code length
  js->code = buf;                         // Point parser to args
  js->clen = n;                           // Set args length
  js->pos = 0;
  //jsoff_t fnlen, fnoff = vstr(js, cbp->jsfunc, &fnlen);
  //printf("CALLING %s %p [%s] -> [%.*s]\n", decl, args, buf, (int) f2len, (char *) &js->mem[f2off]);
  jsval_t res = call_js(js, (char *)&js->mem[f2off], f2len);
  js->code = code, js->clen = clen, js->pos = pos; // Restore parser
  //printf("FFICB->[%s]\n", js_str(js, res));
  switch (decl[1])
  {
  case 'v':
    return mkval(JS_UNDEF, 0);
  case 'i':
    return (long)(is_nan(res) ? 0.0 : tod(res));
  case 'd':
  case 'p':
    return (jw_t)tod(res);
  case 's':
    if (vtype(res) == JS_STR)
      return (jw_t)(js->mem + vstr(js, res, NULL));
  }
  return res;
}

void ffiinitcbargs(union ffi_val *args, jw_t w1, jw_t w2, jw_t w3, jw_t w4, jw_t w5, jw_t w6)
{
  args[0].w = w1;
  args[1].w = w2;
  args[2].w = w3;
  args[3].w = w4;
  args[4].w = w5;
  args[5].w = w6;
}

static jsval_t fficb1(jw_t w1, jw_t w2, jw_t w3, jw_t w4, jw_t w5, jw_t w6)
{
  union ffi_val args[6];
  ffiinitcbargs(args, w1, w2, w3, w4, w5, w6);
  return fficb(w1, args);
}

static jsval_t fficb2(jw_t w1, jw_t w2, jw_t w3, jw_t w4, jw_t w5, jw_t w6)
{
  union ffi_val args[6];
  ffiinitcbargs(args, w1, w2, w3, w4, w5, w6);
  return fficb(w2, args);
}

static jsval_t fficb3(jw_t w1, jw_t w2, jw_t w3, jw_t w4, jw_t w5, jw_t w6)
{
  union ffi_val args[6];
  ffiinitcbargs(args, w1, w2, w3, w4, w5, w6);
  return fficb(w3, args);
}

static jsval_t fficb4(jw_t w1, jw_t w2, jw_t w3, jw_t w4, jw_t w5, jw_t w6)
{
  union ffi_val args[6];
  ffiinitcbargs(args, w1, w2, w3, w4, w5, w6);
  return fficb(w4, args);
}

static jsval_t fficb5(jw_t w1, jw_t w2, jw_t w3, jw_t w4, jw_t w5, jw_t w6)
{
  union ffi_val args[6];
  ffiinitcbargs(args, w1, w2, w3, w4, w5, w6);
  return fficb(w5, args);
}

static jsval_t fficb6(jw_t w1, jw_t w2, jw_t w3, jw_t w4, jw_t w5, jw_t w6)
{
  union ffi_val args[6];
  ffiinitcbargs(args, w1, w2, w3, w4, w5, w6);
  return fficb(w6, args);
}

w6w_t setfficb(const char *decl, int *idx)
{
  w6w_t res = 0, cbs[] = {fficb1, fficb2, fficb3, fficb4, fficb5, fficb6};
  for (size_t i = 1; decl[i] != '\0' && decl[i] != ']'; i++)
  {
    if (i >= (sizeof(cbs) / sizeof(cbs[0])))
      break;
    if (decl[i] == 'u')
      res = cbs[i - 1]; //, printf("SET CB %zu\n", i - 1);
    (*idx)++;
  }
  (*idx) += 2;
  return res;
}
#endif

// Call native C function
jsval_t call_c(JS *js, const char *fn, int fnlen, jsoff_t fnoff)
{
  union ffi_val args[JS_CFG_MAX_FFI_ARGS], res;
  jsoff_t cbp = 0;
  int n = 0, i, type = fn[0] == 'd' ? 1 : 0;
  for (i = 1; i < fnlen && fn[i] != '@' && n < JS_CFG_MAX_FFI_ARGS; i++)
  {
    js->pos = skiptonext(js->code, js->clen, js->pos);
    if (js->pos >= js->clen)
      return js_err(js, "bad arg %d", n + 1);
    jsval_t v = resolveprop(js, js_expr(js, JS_OP_COMMA, JS_TK_RPAREN));
    // printf("  arg %d[%c] -> %s\n", n, fn[i], js_str(js, v));
    if (fn[i] == 'd' || (fn[i] == 'j' && sizeof(jsval_t) > sizeof(jw_t)))
    {
      type |= 1 << (n + 1);
    }
    uint8_t t = vtype(v);
    switch (fn[i])
    {
#ifndef JS_NOCB
    case '[':
      // Create a non-GC-able FFI callback parameter inside JS runtime memory,
      // to make it live if C calls it after GC is performed. This param
      // should be a `void *` that goes to C, and stays intact. We allocate
      // it at the end of free memory block and never GC. It has 3 values:
      //    offset to the js->mem. Used by fficb() to obtain `JS *` ptr
      //    offset of the caller func. Used to get C func signature
      //    offset of the cb func. Used to actually run JS callback
      js->ncbs++;
      js->size -= sizeof(jsoff_t) * 3;
      cbp = js->size;
      saveoff(js, cbp, cbp);
      saveoff(js, cbp + sizeof(jsoff_t), fnoff);
      saveoff(js, cbp + sizeof(jsoff_t) + sizeof(jsoff_t), vdata(v));
      args[n++].p = (void *)setfficb(&fn[i + 1], &i);
      //printf("CB PARAM SET: js %p, param %p %u\n", js, &js->mem[cbp], cbp);
      break;
#endif
    case 'd':
      if (t != JS_NUM)
        return js_err(js, "bad arg %d", n + 1);
      args[n++].d = tod(v);
      break;
    case 'b':
      if (t != JS_BOOL)
        return js_err(js, "bad arg %d", n + 1);
      args[n++].w = vdata(v);
      break;
    case 'i':
      if (t != JS_NUM && t != JS_BOOL)
        return js_err(js, "bad arg %d", n + 1);
      args[n++].w = t == JS_BOOL ? (long)vdata(v) : (long)tod(v);
      break;
    case 's':
      if (t != JS_STR)
        return js_err(js, "bad arg %d", n + 1);
      args[n++].p = js->mem + vstr(js, v, NULL);
      break;
    case 'p':
      if (t != JS_NUM)
        return js_err(js, "bad arg %d", n + 1);
      args[n++].w = (jw_t)tod(v);
      break;
    case 'j':
      args[n++].u64 = v;
      break;
    case 'm':
      args[n++].p = js;
      break;
    case 'u':
      args[n++].p = &js->mem[cbp];
      break;
    default:
      return js_err(js, "bad sig");
    }
    js->pos = skiptonext(js->code, js->clen, js->pos);
    if (js->pos < js->clen && js->code[js->pos] == ',')
      js->pos++;
  }
  uintptr_t f = (uintptr_t)unhexn((uint8_t *)&fn[i + 1], fnlen - i - 1);
  //printf("  type %d nargs %d addr %" PRIxPTR "\n", type, n, f);
  if (js->pos != js->clen)
    return js_err(js, "num args");
  if (fn[i] != '@')
    return js_err(js, "ffi");
  if (f == 0)
    return js_err(js, "ffi");
#ifndef WIN32
#define __cdecl
#endif
  switch (type)
  {
  case 0:
    res.u64 = ((uint64_t(__cdecl *)(jw_t, jw_t, jw_t, jw_t, jw_t, jw_t))f)(args[0].w, args[1].w, args[2].w, args[3].w, args[4].w, args[5].w);
    break;
  case 1:
    res.d = ((double(__cdecl *)(jw_t, jw_t, jw_t, jw_t, jw_t, jw_t))f)(args[0].w, args[1].w, args[2].w, args[3].w, args[4].w, args[5].w);
    break;
  case 2:
    res.u64 = ((uint64_t(__cdecl *)(double, jw_t, jw_t, jw_t, jw_t, jw_t))f)(args[0].d, args[1].w, args[2].w, args[3].w, args[4].w, args[5].w);
    break;
  case 3:
    res.d = ((double(__cdecl *)(double, jw_t, jw_t, jw_t, jw_t, jw_t))f)(args[0].d, args[1].w, args[2].w, args[3].w, args[4].w, args[5].w);
    break;
  case 4:
    res.u64 = ((uint64_t(__cdecl *)(jw_t, double, jw_t, jw_t, jw_t, jw_t))f)(args[0].w, args[1].d, args[2].w, args[3].w, args[4].w, args[5].w);
    break;
  case 5:
    res.d = ((double(__cdecl *)(jw_t, double, jw_t, jw_t, jw_t, jw_t))f)(args[0].w, args[1].d, args[2].w, args[3].w, args[4].w, args[5].w);
    break;
  case 6:
    res.u64 = ((uint64_t(__cdecl *)(double, double, jw_t, jw_t, jw_t, jw_t))f)(args[0].d, args[1].d, args[2].w, args[3].w, args[4].w, args[5].w);
    break;
  case 7:
    res.d = ((double(__cdecl *)(double, double, jw_t, jw_t, jw_t, jw_t))f)(args[0].d, args[1].d, args[2].w, args[3].w, args[4].w, args[5].w);
    break;
  default:
    return js_err(js, "ffi");
  }
  //printf("  TYPE %d RES: %" PRIxPTR " %g %p\n", type, res.v, res.d, res.p);
  // Import return value into JS
  switch (fn[0])
  {
  case 'p':
    return tov(res.w);
  case 'i':
    return tov((int)res.u64);
  case 'd':
    return tov(res.d);
  case 'b':
    return mkval(JS_BOOL, res.w ? 1 : 0);
  case 's':
    return mkstr(js, (char *)(intptr_t)res.w, strlen((char *)(intptr_t)res.w));
  case 'v':
    return mkval(JS_UNDEF, 0);
  case 'j':
    return (jsval_t)res.u64;
  }
  return js_err(js, "bad sig");
}