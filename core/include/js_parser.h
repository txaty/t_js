#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "js_types.h"

#define TOK(T, LEN)       \
    {                     \
        js->tok = T;      \
        js->tlen = (LEN); \
        break;            \
    }

#define LOOK(OFS, CH) js->toff + OFS < js->clen &&buf[OFS] == CH

bool is_space(int c);
bool is_digit(int c);
bool is_xdigit(int c);
bool is_alpha(int c);
bool is_ident_begin(int c);
bool is_ident_continue(int c);
bool is_err(jsval_t v);
bool is_op(uint8_t tok);
bool is_unary(uint8_t tok);
bool is_right_assoc(uint8_t tok);
bool is_assign(uint8_t tok);

jsoff_t skiptonext(const char *code, jsoff_t len, jsoff_t n);
uint8_t parsekeyword(const char *buf, size_t len);
uint8_t parseident(const char *buf, jsoff_t len, jsoff_t *tlen);
jsval_t js_str_literal(JS *js);
jsval_t js_obj_literal(JS *js);
jsval_t js_func_literal(JS *js);
jsval_t js_expr(JS *js, uint8_t etok, uint8_t etok2);
uint8_t nexttok(JS *js);
uint8_t lookahead(JS *js);
void sortops(uint8_t *ops, int nops, jsval_t *stk);
