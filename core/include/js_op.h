#pragma once
#include "js_types.h"

jsval_t assign(JS *js, jsval_t lhs, jsval_t val);
jsval_t do_string_op(JS *js, uint8_t op, jsval_t l, jsval_t r);
jsval_t do_dot_op(JS *js, jsval_t l, jsval_t r);
jsval_t do_call_op(JS *js, jsval_t func, jsval_t args);
jsval_t do_op(JS *js, uint8_t op, jsval_t lhs, jsval_t rhs);
