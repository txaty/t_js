#pragma once
#include <stdio.h>
#include "js_types.h"

void mkscope(JS *js);
void delscope(JS *js);
jsval_t lookup(JS *js, const char *buf, size_t len);