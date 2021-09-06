#pragma once
#include "js_types.h"

jsval_t setprop(JS *js, jsval_t obj, jsval_t k, jsval_t v);
jsval_t resolveprop(JS *js, jsval_t v);