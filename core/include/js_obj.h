#pragma once
#include <stdio.h>
#include "js_types.h"

jsval_t mkobj(JS *js, jsoff_t parent);
jsoff_t lkp(JS *js, jsval_t obj, const char *buf, size_t len);
