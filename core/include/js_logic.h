#pragma once
#include <stdbool.h>
#include "js_types.h"

bool js_truthy(JS *js, jsval_t v);
jsval_t js_if(JS *js);
jsval_t js_while(JS *js);
jsval_t js_break(JS *js);
jsval_t js_continue(JS *js);
jsval_t do_logical_or(JS *js, jsval_t l, jsval_t r);