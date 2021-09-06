#pragma once
#include "js_types.h"

#ifndef JS_CFG_MAX_FFI_ARGS
#define JS_CFG_MAX_FFI_ARGS 6u
#endif

typedef uintptr_t jw_t;
typedef jsval_t (*w6w_t)(jw_t, jw_t, jw_t, jw_t, jw_t, jw_t);
union ffi_val
{
  void *p;
  w6w_t fp;
  jw_t w;
  double d;
  uint64_t u64;
};

jw_t fficb(uintptr_t param, union ffi_val *args);
void ffiinitcbargs(union ffi_val *args, jw_t w1, jw_t w2, jw_t w3, jw_t w4, jw_t w5, jw_t w6);
w6w_t setfficb(const char *decl, int *idx);
jsval_t call_c(JS *js, const char *fn, int fnlen, jsoff_t fnoff);
