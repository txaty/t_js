#include "js_stmt.h"
#include "js_include.h"

jsval_t js_block_or_stmt(JS *js)
{
    js->pos = skiptonext(js->code, js->clen, js->pos);
    if (js->pos < js->clen && js->code[js->pos] == '{')
    {
        js->pos++;
        return js_block(js, !(js->flags & F_NOEXEC));
    }
    else
    {
        return resolveprop(js, js_stmt(js, JS_TK_SEMICOLON));
    }
}

jsval_t js_stmt(JS *js, uint8_t etok)
{
    jsval_t res;
    if (js->lev == 0)
        js_gc(js); // Before top-level stmt, garbage collect
    js->lev++;
    switch (nexttok(js))
    {
    case JS_KW_CASE:
    case JS_KW_CATCH:
    case JS_KW_CLASS:
    case JS_KW_CONST:
    case JS_KW_DEFAULT:
    case JS_KW_DELETE:
    case JS_KW_DO:
    case JS_KW_FINALLY:
    case JS_KW_FOR:
    case JS_KW_IN:
    case JS_KW_INSTANCEOF:
    case JS_KW_NEW:
    case JS_KW_SWITCH:
    case JS_KW_THIS:
    case JS_KW_THROW:
    case JS_KW_TRY:
    case JS_KW_VAR:
    case JS_KW_VOID:
    case JS_KW_WITH:
    case JS_KW_YIELD:
        res = js_err(js, "'%.*s' not implemented", (int)js->tlen, js->code + js->toff);
        break;
    case JS_KW_CONTINUE:
        res = js_continue(js);
        break;
    case JS_KW_BREAK:
        res = js_break(js);
        break;
    case JS_KW_LET:
        res = js_let(js);
        break;
    case JS_KW_IF:
        res = js_if(js);
        break;
    case JS_TK_LBRACE:
        res = js_block(js, !(js->flags & F_NOEXEC));
        break;
    case JS_KW_WHILE:
        res = js_while(js);
        break;
    case JS_KW_RETURN:
        res = js_return(js);
        break;
    default:
        js->pos -= js->tlen; // Unparse last parsed token
        res = resolveprop(js, js_expr(js, etok, JS_TK_SEMICOLON));
        break;
    }
    js->lev--;
    return res;
}