#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "js_types.h"

jsval_t mkstr(JS *js, const void *ptr, size_t len);
bool streq(const char *buf, size_t len, const char *p, size_t n);