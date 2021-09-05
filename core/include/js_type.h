#pragma once

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
    JS_TK_ERR = 100,
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
    JS_KW_BREAK = 200,
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
    JS_OP_DOT = 300,
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