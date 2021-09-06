#pragma once
#include <stdio.h>
#include "js_types.h"

jsval_t mkentity(JS *js, jsoff_t b, const void *buf, size_t len);
jsoff_t esize(jsoff_t w);