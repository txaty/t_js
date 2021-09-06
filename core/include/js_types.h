#pragma once
#include <stdint.h>

#define F_NOEXEC 1u  // Parse code, but not execute
#define F_LOOP 2u    // We're inside the loop
#define F_CALL 4u    // We're inside a function call
#define F_BREAK 8u   // Exit the loop
#define F_RETURN 16u // Return has been executed

#ifndef JS_CFG_EXPR_MAX
#define JS_CFG_EXPR_MAX 20u
#endif

typedef uint64_t jsval_t; // JS value placeholder
typedef uint32_t jsoff_t;

// A JS memory stores diffenent entities: objects, properties, strings
// All entities are packed to the beginning of a buffer.
// The `brk` marks the end of the used memory:
//
//    | entity1 | entity2| .... |entityN|         unused memory        | cbs |
//    |---------|--------|------|-------|------------------------------|-----|
//  js.mem                           js.brk                        js.size
//
//  Each entity is 4-byte aligned, therefore 2 LSB bits store entity type.
//  Object:   8 bytes: offset of the first property, offset of the upper obj
//  Property: 8 bytes + val: 4 byte next prop, 4 byte key offs, N byte value
//  String:   4xN bytes: 4 byte len << 2, 4byte-aligned 0-terminated data
//
// FFI userdata callback pointers "cbs" are placed past js.size. Since they
// are passed to the user's C code, they stay constant and are not GC-ed

// JS engine (opaque)
typedef struct js_struct
{
    const char *code; // Currently parsed code snippet
    char errmsg[36];  // Error message placeholder
    uint8_t tok;      // Last parsed token value
    uint8_t flags;    // Execution flags, see F_* enum below
    uint16_t lev;     // Recursion level
    jsoff_t clen;     // Code snippet length
    jsoff_t pos;      // Current parsing position
    jsoff_t toff;     // Offset of the last parsed token
    jsoff_t tlen;     // Length of the last parsed token
    jsval_t tval;     // Holds last parsed numeric or string literal value
    jsval_t scope;    // Current scope
    uint8_t *mem;     // Available JS memory
    jsoff_t size;     // Memory size
    jsoff_t brk;      // Current mem usage boundary
    jsoff_t ncbs;     // Number of FFI-ed C "userdata" callback pointers
} JS;

// types
// IMPORTANT: JS_OBJ, JS_PROP, JS_STR must go first. That is required by the
// memory layout functions: memory entity types are encoded in the 2 bits,
// thus type values must be 0,1,2,3
typedef enum js_type_en
{
    JS_OBJ,
    JS_PROP,
    JS_STR,
    JS_UNDEF,
    JS_NULL,
    JS_NUM,
    JS_BOOL,
    JS_FUNC,
    JS_CODEREF,
    JS_ERR
} js_t;

// parsed tokens
typedef enum js_tk_en
{
    JS_TK_ERR = 10u,
    JS_TK_EOF,
    JS_TK_IDENTIFIER,
    JS_TK_NUMBER,
    JS_TK_STRING,
    JS_TK_SEMICOLON,
    JS_TK_LPAREN,
    JS_TK_RPAREN,
    JS_TK_LBRACE,
    JS_TK_RBRACE,
} js_tk_t;

// keywords
typedef enum js_kw_en
{
    JS_KW_BREAK = 20u,
    JS_KW_CASE,
    JS_KW_CATCH,
    JS_KW_CLASS,
    JS_KW_CONST,
    JS_KW_CONTINUE,
    JS_KW_DEFAULT,
    JS_KW_DELETE,
    JS_KW_DO,
    JS_KW_ELSE,
    JS_KW_FINALLY,
    JS_KW_FOR,
    JS_KW_FUNC,
    JS_KW_IF,
    JS_KW_IN,
    JS_KW_INSTANCEOF,
    JS_KW_LET,
    JS_KW_NEW,
    JS_KW_RETURN,
    JS_KW_SWITCH,
    JS_KW_THIS,
    JS_KW_THROW,
    JS_KW_TRY,
    JS_KW_VAR,
    JS_KW_VOID,
    JS_KW_WHILE,
    JS_KW_WITH,
    JS_KW_YIELD,
    JS_KW_UNDEF,
    JS_KW_NULL,
    JS_KW_TRUE,
    JS_KW_FALSE,
} js_kw_t;

// operators
typedef enum js_op_en
{
    JS_OP_DOT = 80u,
    JS_OP_CALL,
    JS_OP_POSTINC,
    JS_OP_POSTDEC,
    JS_OP_NOT,
    JS_OP_NEG,
    JS_OP_TYPEOF,
    JS_OP_UPLUS,
    JS_OP_UMINUS,
    JS_OP_EXP,
    JS_OP_MUL,
    JS_OP_DIV,
    JS_OP_REM,
    JS_OP_PLUS,
    JS_OP_MINUS,
    JS_OP_SHL,
    JS_OP_SHR,
    JS_OP_ZSHR,
    JS_OP_LT,
    JS_OP_LE,
    JS_OP_GT,
    JS_OP_GE,
    JS_OP_EQ,
    JS_OP_NE,
    JS_OP_AND,
    JS_OP_XOR,
    JS_OP_OR,
    JS_OP_LAND,
    JS_OP_LOR,
    JS_OP_COLON,
    JS_OP_Q,
    JS_OP_ASSIGN,
    JS_OP_PLUS_ASSIGN,
    JS_OP_MINUS_ASSIGN,
    JS_OP_MUL_ASSIGN,
    JS_OP_DIV_ASSIGN,
    JS_OP_REM_ASSIGN,
    JS_OP_SHL_ASSIGN,
    JS_OP_SHR_ASSIGN,
    JS_OP_ZSHR_ASSIGN,
    JS_OP_AND_ASSIGN,
    JS_OP_XOR_ASSIGN,
    JS_OP_OR_ASSIGN,
    JS_OP_COMMA,
} js_op_t;