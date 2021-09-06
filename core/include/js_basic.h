#pragma once
#include <stdint.h>
#include "js_types.h"

uint8_t unhex(uint8_t c);
uint64_t unhexn(const uint8_t *s, int len);
jsoff_t offtolen(jsoff_t off);
jsoff_t vstrlen(JS *js, jsval_t v);
const char *typestr(uint8_t t);
jsval_t upper(JS *js, jsval_t scope);
jsoff_t align32(jsoff_t v);
jsoff_t vstr(JS *js, jsval_t value, jsoff_t *len);