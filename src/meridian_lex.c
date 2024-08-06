#include "meridian_lex.h"

#include <stdlib.h>

Lexer Lexer_new(String src) {
    return (Lexer) {
        .src = src,
        .current = 0,
        .line = 1,
        .loc = 1,

        .tokens = NULL,
        .tokensLen = 0,
        .tokensAllocated = 0,
    };
}
void Lexer_free(Lexer *lexer) {
    free(lexer->tokens);
    
    *lexer = Lexer_new(lexer->src);
}

static bool IsNumber(char c) { return c >= '0' && c <= '9'; }
static bool IsLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool IsUniqueCharacter(char c) {
    switch(c) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '!':
    case '?':
    case '_':
        return true;
    default: return false;
    }
}

static bool IsIdentStarter(char c) { return IsLetter(c) || IsUniqueCharacter(c); }
static bool IsIdentChar(char c) { return IsIdentStarter(c) || IsNumber(c); }

static char Lexer_current(Lexer* lexer) {
    return lexer->src.raw[lexer->current];
}

static void Lexer_advance(Lexer *lexer) { lexer->current++; }

static bool Lexer_match(Lexer* lexer, char expected) {
    return Lexer_current(lexer) == expected;
}

void Lexer_run(Lexer *lexer) {
    while(lexer->current < lexer->src.len) {
        char c = Lexer_current(lexer);

        switch(c) {
        case '(':
            while(!Lexer_match(lexer, ')')) {
                Lexer_advance(lexer);
            }
            break;
        default:
            if(IsIdentStarter(c)) {
                u64 start = lexer->current, len = 0;
                while(IsIdentChar(Lexer_current(lexer))) {
                    Lexer_advance(lexer);
                    len++;
                }
            }
            break;
        }
    }
}
