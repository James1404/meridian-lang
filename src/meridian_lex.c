#include "meridian_lex.h"
#include "meridian_tokens.h"

#include <stdio.h>
#include <stdlib.h>

Lexer Lexer_new(String src) {
    return (Lexer) {
        .src = src,
        .current = 0,
        .line = 1,
        .lineOffset = 1,

        .tokens = NULL,
        .tokensLen = 0,
        .tokensAllocated = 8,
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
    case '>':
    case '<':
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

static void Lexer_add(Lexer* lexer, Token token) {
    if(!lexer->tokens) {
        lexer->tokensAllocated = 8;
        lexer->tokens = malloc(sizeof(*lexer->tokens) * lexer->tokensAllocated);
    }

    u32 idx = lexer->tokensLen++;

    
}

void Lexer_run(Lexer *lexer) {
    while(lexer->current < lexer->src.len) {
        char c = Lexer_current(lexer);

        u32 start = lexer->current;

        switch(c) {
        case '(':
            while(!Lexer_match(lexer, ')')) {
                Lexer_advance(lexer);
            }
            break;
            
        case '#':
            while(Lexer_current(lexer) != '\n') Lexer_advance(lexer);
            break;
            
        case '\n':
            lexer->line++;
            lexer->lineOffset = 0;
        case ' ':
        case '\t':
        case '\r':
            Lexer_advance(lexer);
            break;
            
        default:
            if(IsIdentStarter(c)) {
                while(IsIdentChar(Lexer_current(lexer))) {
                    Lexer_advance(lexer);
                }

                Token t = {
                    .start = start,
                    .len = lexer->current - start,
                    .line = lexer->line,
                    .lineOffset = lexer->lineOffset,
                    .tt = TOKEN_IDENT
                };

                t.tt = Meridian_GetKeyword(lexer->src, t);

                Lexer_add(lexer, t);

                continue;
            }

            if(IsNumber(c)) {
                TokenType ty = TOKEN_INT;
                do {
                    Lexer_advance(lexer);
                    c = Lexer_current(lexer);

                    if(c == '.') {
                        ty = TOKEN_FLOAT;
                        continue;
                    }

                    if(IsNumber(c)) {
                        continue;
                    }

                    break;
                } while (true);
                
                Lexer_add(lexer, (Token) {
                        .start = start,
                        .len = lexer->current - start,
                        .line = lexer->line,
                        .lineOffset = lexer->lineOffset,
                        .tt = ty
                    });

                continue;
            }

            Lexer_advance(lexer);
            break;
        }
    }
}


void Lexer_print(Lexer *lexer) {
    printf("<-- Lexer :: { tokensLen = %lu; } -->\n", lexer->tokensLen);
    for(u32 i = 0; i < lexer->tokensLen; i++) {
        Token t = lexer->tokens[i];

        printf("Token :: { tt = %i; line = %i; }\n", t.tt, t.line);
    }
}
