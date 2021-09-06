#pragma once
#include "js_types.h"

jsval_t call_js(JS *js, const char *fn, int fnlen);
jsval_t js_return(JS *js);
jsval_t js_call_params(JS *js);
jsval_t js_eval_nogc(JS *js, const char *buf, jsoff_t len);