#pragma once
#include "js_types.h"

jsval_t js_block_or_stmt(JS *js);
jsval_t js_stmt(JS *js, uint8_t etok);