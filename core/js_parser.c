#include <stdlib.h>
#include "js_parser.h"
#include "js_include.h"

bool is_space(int c)
{
    return c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\f' || c == '\v';
}

bool is_digit(int c)
{
    return c >= '0' && c <= '9';
}

bool is_xdigit(int c)
{
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool is_alpha(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_ident_begin(int c)
{
    return c == '_' || c == '$' || is_alpha(c);
}

bool is_ident_continue(int c)
{
    return c == '_' || c == '$' || is_alpha(c) || is_digit(c);
}

bool is_err(jsval_t v)
{
    return vtype(v) == JS_ERR;
}

bool is_op(uint8_t tok)
{
    return tok >= JS_OP_DOT;
}

bool is_unary(uint8_t tok)
{
    return tok >= JS_OP_POSTINC && tok <= JS_OP_UMINUS;
}

bool is_right_assoc(uint8_t tok)
{
    return (tok >= JS_OP_NOT && tok <= JS_OP_UMINUS) || (tok >= JS_OP_Q && tok <= JS_OP_OR_ASSIGN);
}

bool is_assign(uint8_t tok)
{
    return (tok >= JS_OP_ASSIGN && tok <= JS_OP_OR_ASSIGN);
}

// Skip whitespaces and comments
jsoff_t skiptonext(const char *code, jsoff_t len, jsoff_t n)
{
    // printf("SKIP: [%.*s]\n", len - n, &code[n]);
    while (n < len)
    {
        if (is_space(code[n]))
        {
            n++;
        }
        else if (n + 1 < len && code[n] == '/' && code[n + 1] == '/')
        {
            for (n += 2; n < len && code[n] != '\n';)
                n++;
        }
        else if (n + 3 < len && code[n] == '/' && code[n + 1] == '*')
        {
            for (n += 4; n < len && (code[n - 2] != '*' || code[n - 1] != '/');)
                n++;
        }
        else
        {
            break;
        }
    }
    return n;
}

uint8_t parsekeyword(const char *buf, size_t len)
{
    switch (buf[0])
    {
    case 'b':
        if (streq("break", 5, buf, len))
            return JS_KW_BREAK;
        break;
    case 'c':
        if (streq("class", 5, buf, len))
            return JS_KW_CLASS;
        if (streq("case", 4, buf, len))
            return JS_KW_CASE;
        if (streq("catch", 5, buf, len))
            return JS_KW_CATCH;
        if (streq("const", 5, buf, len))
            return JS_KW_CONST;
        if (streq("continue", 8, buf, len))
            return JS_KW_CONTINUE;
        break;
    case 'd':
        if (streq("do", 2, buf, len))
            return JS_KW_DO;
        if (streq("default", 7, buf, len))
            return JS_KW_DEFAULT;
        break; // if (streq("delete", 6, buf, len)) return TOK_DELETE; break;
    case 'e':
        if (streq("else", 4, buf, len))
            return JS_KW_ELSE;
        break;
    case 'f':
        if (streq("for", 3, buf, len))
            return JS_KW_FOR;
        if (streq("function", 8, buf, len))
            return JS_KW_FUNC;
        if (streq("finally", 7, buf, len))
            return JS_KW_FINALLY;
        if (streq("false", 5, buf, len))
            return JS_KW_FALSE;
        break;
    case 'i':
        if (streq("if", 2, buf, len))
            return JS_KW_IF;
        if (streq("in", 2, buf, len))
            return JS_KW_IN;
        if (streq("instanceof", 10, buf, len))
            return JS_KW_INSTANCEOF;
        break;
    case 'l':
        if (streq("let", 3, buf, len))
            return JS_KW_LET;
        break;
    case 'n':
        if (streq("new", 3, buf, len))
            return JS_KW_NEW;
        if (streq("null", 4, buf, len))
            return JS_KW_NULL;
        break;
    case 'r':
        if (streq("return", 6, buf, len))
            return JS_KW_RETURN;
        break;
    case 's':
        if (streq("switch", 6, buf, len))
            return JS_KW_SWITCH;
        break;
    case 't':
        if (streq("try", 3, buf, len))
            return JS_KW_TRY;
        if (streq("this", 4, buf, len))
            return JS_KW_THIS;
        if (streq("throw", 5, buf, len))
            return JS_KW_THROW;
        if (streq("true", 4, buf, len))
            return JS_KW_TRUE;
        if (streq("typeof", 6, buf, len))
            return JS_OP_TYPEOF;
        break;
    case 'u':
        if (streq("undefined", 9, buf, len))
            return JS_KW_UNDEF;
        break;
    case 'v':
        if (streq("var", 3, buf, len))
            return JS_KW_VAR;
        if (streq("void", 4, buf, len))
            return JS_KW_VOID;
        break;
    case 'w':
        if (streq("while", 5, buf, len))
            return JS_KW_WHILE;
        if (streq("with", 4, buf, len))
            return JS_KW_WITH;
        break;
    case 'y':
        if (streq("yield", 5, buf, len))
            return JS_KW_YIELD;
        break;
    }
    return JS_TK_IDENTIFIER;
}

uint8_t parseident(const char *buf, jsoff_t len, jsoff_t *tlen)
{
    if (is_ident_begin(buf[0]))
    {
        while (*tlen < len && is_ident_continue(buf[*tlen]))
            (*tlen)++;
        return parsekeyword(buf, *tlen);
    }
    return JS_TK_ERR;
}

jsval_t js_str_literal(JS *js)
{
    uint8_t *in = (uint8_t *)&js->code[js->toff];
    uint8_t *out = &js->mem[js->brk + sizeof(jsoff_t)];
    int n1 = 0, n2 = 0;
    // printf("STR %u %lu %lu\n", js->brk, js->tlen, js->clen);
    if (js->brk + sizeof(jsoff_t) + js->tlen > js->size)
        return js_err(js, "oom");
    while (n2++ + 2 < (int)js->tlen)
    {
        if (in[n2] == '\\')
        {
            if (in[n2 + 1] == in[0])
            {
                out[n1++] = in[0];
            }
            else if (in[n2 + 1] == 'n')
            {
                out[n1++] = '\n';
            }
            else if (in[n2 + 1] == 't')
            {
                out[n1++] = '\t';
            }
            else if (in[n2 + 1] == 'r')
            {
                out[n1++] = '\r';
            }
            else if (in[n2 + 1] == 'x' && is_xdigit(in[n2 + 2]) &&
                     is_xdigit(in[n2 + 3]))
            {
                out[n1++] = unhex(in[n2 + 2]) << 4 | unhex(in[n2 + 3]);
                n2 += 2;
            }
            else
            {
                return js_err(js, "bad str literal");
            }
            n2++;
        }
        else
        {
            out[n1++] = js->code[js->toff + n2];
        }
    }
    return mkstr(js, NULL, n1);
}

jsval_t js_obj_literal(JS *js)
{
    uint8_t exe = !(js->flags & F_NOEXEC);
    // printf("OLIT1\n");
    jsval_t obj = exe ? mkobj(js, 0) : mkval(JS_UNDEF, 0);
    if (is_err(obj))
        return obj;
    while (nexttok(js) != JS_TK_RBRACE)
    {
        if (js->tok != JS_TK_IDENTIFIER)
            return js_err(js, "parse error");
        size_t koff = js->toff, klen = js->tlen;
        if (nexttok(js) != JS_OP_COLON)
            return js_err(js, "parse error");
        jsval_t val = js_expr(js, JS_TK_RBRACE, JS_OP_COMMA);
        if (exe)
        {
            // printf("XXXX [%s] scope: %lu\n", js_str(js, val), vdata(js->scope));
            if (is_err(val))
                return val;
            jsval_t key = mkstr(js, js->code + koff, klen);
            if (is_err(key))
                return key;
            jsval_t res = setprop(js, obj, key, resolveprop(js, val));
            if (is_err(res))
                return res;
        }
        if (js->tok == JS_TK_RBRACE)
            break;
        if (js->tok != JS_OP_COMMA)
            return js_err(js, "parse error");
    }
    return obj;
}

jsval_t js_func_literal(JS *js)
{
    jsoff_t pos = js->pos;
    uint8_t flags = js->flags; // Save current flags
    if (nexttok(js) != JS_TK_LPAREN)
        return js_err(js, "parse error");
    for (bool expect_ident = false; nexttok(js) != JS_TK_EOF; expect_ident = true)
    {
        if (expect_ident && js->tok != JS_TK_IDENTIFIER)
            return js_err(js, "parse error");
        if (js->tok == JS_TK_RPAREN)
            break;
        if (js->tok != JS_TK_IDENTIFIER)
            return js_err(js, "parse error");
        if (nexttok(js) == JS_TK_RPAREN)
            break;
        if (js->tok != JS_OP_COMMA)
            return js_err(js, "parse error");
    }
    if (js->tok != JS_TK_RPAREN)
        return js_err(js, "parse error");
    if (nexttok(js) != JS_TK_LBRACE)
        return js_err(js, "parse error");
    js->flags |= F_NOEXEC;             // Set no-execution flag to parse the
    jsval_t res = js_block(js, false); // Skip function body - no exec
    if (is_err(res))
        return res;    // But fail short on parse error
    js->flags = flags; // Restore flags
    jsval_t str = mkstr(js, &js->code[pos], js->pos - pos);
    // printf("FUNC: %u [%.*s]\n", pos, js->pos - pos, &js->code[pos]);
    return mkval(JS_FUNC, vdata(str));
}

jsval_t js_expr(JS *js, uint8_t etok, uint8_t etok2)
{
    jsval_t stk[JS_CFG_EXPR_MAX];                      // parsed values
    uint8_t tok, ops[JS_CFG_EXPR_MAX], pt = JS_TK_ERR; // operator indices
    uint8_t n = 0, nops = 0, nuops = 0;
    // printf("E1 %d %d %d %u/%u\n", js->tok, etok, etok2, js->pos, js->clen);
    while ((tok = nexttok(js)) != etok && tok != etok2 && tok != JS_TK_EOF)
    {
        // printf("E2 %d %d %d %u/%u\n", js->tok, etok, etok2, js->pos, js->clen);
        if (tok == JS_TK_ERR)
            return js_err(js, "parse error");
        if (n >= JS_CFG_EXPR_MAX)
            return js_err(js, "expr too deep");
        // Convert JS_TK_LPAREN to a function call TOK_CALL if required
        if (tok == JS_TK_LPAREN && (n > 0 && !is_op(pt)))
            tok = JS_OP_CALL;
        if (is_op(tok))
        {
            // Convert this plus or minus to unary if required
            if (tok == JS_OP_PLUS || tok == JS_OP_MINUS)
            {
                bool convert =
                    (n == 0) || (is_op(pt) && (!is_unary(pt) || is_right_assoc(pt)));
                if (convert && tok == JS_OP_PLUS)
                    tok = JS_OP_UPLUS;
                if (convert && tok == JS_OP_MINUS)
                    tok = JS_OP_UMINUS;
            }
            ops[nops++] = n;
            stk[n++] = mkval(JS_ERR, tok); // Convert op into value and store
            if (!is_unary(tok))
                nuops++; // Count non-unary ops
            // For function calls, store arguments - but don't evaluate just yet
            if (tok == JS_OP_CALL)
            {
                stk[n++] = js_call_params(js);
                if (is_err(stk[n - 1]))
                    return stk[n - 1];
            }
        }
        else
        {
            switch (tok)
            {
            case JS_TK_IDENTIFIER:
                // Root level identifiers we lookup and push property: "a" -> lookup("a")
                // Identifiers after dot we push as string tokens: "a.b" -> "b"
                // mkcoderef() returns jsval_t that references an (offset,length)
                // inside the parsed code.
                stk[n] = js->flags & F_NOEXEC ? 0 : n > 0 && is_op(vdata(stk[n - 1]) & 255) && vdata(stk[n - 1]) == JS_OP_DOT ? mkcoderef((jsoff_t)js->toff, (jsoff_t)js->tlen)
                                                                                                                              : lookup(js, js->code + js->toff, js->tlen);
                n++;
                break;
            case JS_TK_NUMBER:
                stk[n++] = js->tval;
                break;
            case JS_TK_LBRACE:
                stk[n++] = js_obj_literal(js);
                break;
            case JS_TK_STRING:
                stk[n++] = js_str_literal(js);
                break;
            case JS_KW_FUNC:
                stk[n++] = js_func_literal(js);
                break;
            case JS_KW_NULL:
                stk[n++] = mkval(JS_NULL, 0);
                break;
            case JS_KW_UNDEF:
                stk[n++] = mkval(JS_UNDEF, 0);
                break;
            case JS_KW_TRUE:
                stk[n++] = mkval(JS_BOOL, 1);
                break;
            case JS_KW_FALSE:
                stk[n++] = mkval(JS_BOOL, 0);
                break;
            case JS_TK_LPAREN:
                stk[n++] = js_expr(js, JS_TK_RPAREN, JS_TK_EOF);
                break;
            default:
                return js_err(js, "unexpected token '%.*s'", (int)js->tlen, js->code + js->toff);
            }
        }
        if (!is_op(tok) && is_err(stk[n - 1]))
            return stk[n - 1];
        pt = tok;
    }
    // printf("EE toks=%d ops=%d binary=%d\n", n, nops, nuops);
    if (js->flags & F_NOEXEC)
        return mkval(JS_UNDEF, n); // pass n to the caller
    if (n == 0)
        return mkval(JS_UNDEF, 0);
    if (n != nops + nuops + 1)
        return js_err(js, "bad expr");
    sortops(ops, nops, stk); // Sort operations by priority
    uint32_t mask = 0;
    // uint8_t nq = 0;  // Number of `?` ternary operations
    for (int i = 0; i < nops; i++)
    {
        uint8_t idx = ops[i], op = vdata(stk[idx]) & 255, ri = idx;
        bool unary = is_unary(op), rassoc = is_right_assoc(op);
        bool needleft = unary && rassoc ? false : true;
        bool needright = unary && !rassoc ? false : true;
        jsval_t left = mkval(JS_UNDEF, 0), right = mkval(JS_UNDEF, 0);
        mask |= 1 << idx;
        // printf("  OP: %d idx %d %d%d\n", op, idx, needleft, needright);
        if (needleft)
        {
            if (idx < 1)
                return js_err(js, "bad expr");
            mask |= 1 << (idx - 1);
            ri = getri(mask, idx);
            left = stk[ri];
            if (is_err(left))
                return js_err(js, "bad expr");
        }
        if (needright)
        {
            mask |= 1 << (idx + 1);
            if (idx + 1 >= n)
                return js_err(js, "bad expr");
            right = stk[idx + 1];
            if (is_err(right))
                return js_err(js, "bad expr");
        }
        stk[ri] = do_op(js, op, left, right); // Perform operation
        if (is_err(stk[ri]))
            return stk[ri]; // Propagate error
    }
    return stk[0];
}

uint8_t nexttok(JS *js)
{
    js->tok = JS_TK_ERR;
    js->toff = js->pos = skiptonext(js->code, js->clen, js->pos);
    js->tlen = 0;
    const char *buf = js->code + js->toff;
    if (js->toff >= js->clen)
    {
        js->tok = JS_TK_EOF;
        return js->tok;
    }
    switch (buf[0])
    {
    case '?':
        TOK(JS_OP_Q, 1);
    case ':':
        TOK(JS_OP_COLON, 1);
    case '(':
        TOK(JS_TK_LPAREN, 1);
    case ')':
        TOK(JS_TK_RPAREN, 1);
    case '{':
        TOK(JS_TK_LBRACE, 1);
    case '}':
        TOK(JS_TK_RBRACE, 1);
    case ';':
        TOK(JS_TK_SEMICOLON, 1);
    case ',':
        TOK(JS_OP_COMMA, 1);
    case '!':
        if (LOOK(1, '=') && LOOK(2, '='))
            TOK(JS_OP_NE, 3);
        TOK(JS_OP_NOT, 1);
    case '.':
        TOK(JS_OP_DOT, 1);
    case '~':
        TOK(JS_OP_NEG, 1);
    case '-':
        if (LOOK(1, '-'))
            TOK(JS_OP_POSTDEC, 2);
        if (LOOK(1, '='))
            TOK(JS_OP_MINUS_ASSIGN, 2);
        TOK(JS_OP_MINUS, 1);
    case '+':
        if (LOOK(1, '+'))
            TOK(JS_OP_POSTINC, 2);
        if (LOOK(1, '='))
            TOK(JS_OP_PLUS_ASSIGN, 2);
        TOK(JS_OP_PLUS, 1);
    case '*':
        if (LOOK(1, '*'))
            TOK(JS_OP_EXP, 2);
        if (LOOK(1, '='))
            TOK(JS_OP_MUL_ASSIGN, 2);
        TOK(JS_OP_MUL, 1);
    case '/':
        if (LOOK(1, '='))
            TOK(JS_OP_DIV_ASSIGN, 2);
        TOK(JS_OP_DIV, 1);
    case '%':
        if (LOOK(1, '='))
            TOK(JS_OP_REM_ASSIGN, 2);
        TOK(JS_OP_REM, 1);
    case '&':
        if (LOOK(1, '&'))
            TOK(JS_OP_LAND, 2);
        if (LOOK(1, '='))
            TOK(JS_OP_AND_ASSIGN, 2);
        TOK(JS_OP_AND, 1);
    case '|':
        if (LOOK(1, '|'))
            TOK(JS_OP_LOR, 2);
        if (LOOK(1, '='))
            TOK(JS_OP_OR_ASSIGN, 2);
        TOK(JS_OP_OR, 1);
    case '=':
        if (LOOK(1, '=') && LOOK(2, '='))
            TOK(JS_OP_EQ, 3);
        TOK(JS_OP_ASSIGN, 1);
    case '<':
        if (LOOK(1, '<') && LOOK(2, '='))
            TOK(JS_OP_SHL_ASSIGN, 3);
        if (LOOK(1, '<'))
            TOK(JS_OP_SHL, 2);
        if (LOOK(1, '='))
            TOK(JS_OP_LE, 2);
        TOK(JS_OP_LT, 1);
    case '>':
        if (LOOK(1, '>') && LOOK(2, '='))
            TOK(JS_OP_SHR_ASSIGN, 3);
        if (LOOK(1, '>'))
            TOK(JS_OP_SHR, 2);
        if (LOOK(1, '='))
            TOK(JS_OP_GE, 2);
        TOK(JS_OP_GT, 1);
    case '^':
        if (LOOK(1, '='))
            TOK(JS_OP_XOR_ASSIGN, 2);
        TOK(JS_OP_XOR, 1);
    case '"':
    case '\'':
        js->tlen++;
        while (js->toff + js->tlen < js->clen && buf[js->tlen] != buf[0])
        {
            uint8_t increment = 1;
            if (buf[js->tlen] == '\\')
            {
                if (js->toff + js->tlen + 2 > js->clen)
                    break;
                increment = 2;
                if (buf[js->tlen + 1] == 'x')
                {
                    if (js->toff + js->tlen + 4 > js->clen)
                        break;
                    increment = 4;
                }
            }
            js->tlen += increment;
        }
        if (buf[0] == buf[js->tlen])
            js->tok = JS_TK_STRING, js->tlen++;
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        char *end;
        js->tval = tov(strtod(buf, &end)); // TODO(lsm): protect against OOB access
        TOK(JS_TK_NUMBER, (jsoff_t)(end - buf));
    }
    default:
        js->tok = parseident(buf, js->clen - js->toff, &js->tlen);
        break;
    }
    js->pos = js->toff + js->tlen;
    // printf("NEXT: %d [%.*s]\n", js->tok, (int) js->tlen, buf);
    return js->tok;
}

uint8_t lookahead(JS *js)
{
    uint8_t tok = nexttok(js);
    js->pos -= js->tlen;
    return tok;
}

// Bubble sort operators by their priority. TOK_* enum is already sorted
void sortops(uint8_t *ops, int nops, jsval_t *stk)
{
    uint8_t prios[] = {19, 19, 17, 17, 16, 16, 16, 16, 16, 15, 14, 14, 14, 13, 13,
                       12, 12, 12, 11, 11, 11, 11, 10, 10, 9, 8, 7, 6, 5, 4,
                       4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1};
    // printf("PRIO: %d\n", prios[TOK_PLUS - TOK_DOT]);
    for (bool done = false; done == false;)
    {
        done = true;
        for (int i = 0; i + 1 < nops; i++)
        {
            uint8_t o1 = vdata(stk[ops[i]]) & 255, o2 = vdata(stk[ops[i + 1]]) & 255;
            uint8_t a = prios[o1 - JS_OP_DOT], b = prios[o2 - JS_OP_DOT], tmp = ops[i];
            bool swap = a < b;
            if (o1 == o2 && is_right_assoc(o1) && ops[i] < ops[i + 1])
                swap = 1;
            if (swap)
                ops[i] = ops[i + 1], ops[i + 1] = tmp, done = false;
        }
    }
}