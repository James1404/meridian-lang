#include "meridian_lex.h"
#include "meridian_tokens.h"
#include "meridian_error.h"

#include <stdio.h>
#include <stdlib.h>

Lexer Lexer_new(TokenList* out) {
    return (Lexer) {
        .src = out->src,
        .current = 0,
        .line = 1,
        .lineOffset = 1,

        .tokens = out,
    };
}
void Lexer_free(Lexer *lexer) {
    *lexer = Lexer_new(lexer->tokens);
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
    case '=':
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
    TokenList_push(lexer->tokens, token);
}

static void Lexer_add_single(Lexer* lexer, TokenType tt) {
    Lexer_advance(lexer);
    
    Token t = {
        .tt = tt,
        .start = lexer->start,
        .len = lexer->current - lexer->start,
        .line = lexer->line,
        .lineOffset = lexer->lineOffset,
    };

    Lexer_add(lexer, t);
}

void Lexer_run(Lexer *lexer) {
    while(lexer->current < lexer->src.len) {
        char c = Lexer_current(lexer);

        lexer->start = lexer->current;

        switch(c) {
        case '(': Lexer_add_single(lexer, TOKEN_LPAREN); break;
        case ')': Lexer_add_single(lexer, TOKEN_RPAREN); break;

        case '{': Lexer_add_single(lexer, TOKEN_LBRACE); break;
        case '}': Lexer_add_single(lexer, TOKEN_RBRACE); break;

        case '[': Lexer_add_single(lexer, TOKEN_LBRACKET); break;
        case ']': Lexer_add_single(lexer, TOKEN_RBRACKET); break;

        case ';': Lexer_add_single(lexer, TOKEN_SEMICOLON); break;
        case ':': Lexer_add_single(lexer, TOKEN_COLON); break;
            
        case '"':
            Lexer_advance(lexer);
            while(!Lexer_match(lexer, '"')) Lexer_advance(lexer);

            Token t = {
                .start = lexer->start + 1,
                .len = lexer->current - lexer->start - 1,
                .tt = TOKEN_STRING,
                .line = lexer->line,
                .lineOffset = lexer->lineOffset,
            };
            
            Lexer_advance(lexer);
            Lexer_add(lexer, t);
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
                    .start = lexer->start,
                    .len = lexer->current - lexer->start,
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
                        .start = lexer->start,
                        .len = lexer->current - lexer->start,
                        .line = lexer->line,
                        .lineOffset = lexer->lineOffset,
                        .tt = ty
                    });

                continue;
            }

            Meridian_error("Unknown Characted '%c'", c);
            Lexer_advance(lexer);
            break;
        }
    }
}

void Lexer_print(Lexer *lexer) {
    printf("<-- Lexer :: { tokensLen = %lu; } -->\n", lexer->tokens->len);
    for(u32 i = 0; i < lexer->tokens->len; i++) {
        Token t = lexer->tokens->data[i];

        char* text = lexer->src.raw + t.start;
        u32 textLen = t.len;

        printf("Token :: { text = \"%.*s\"; tt = %i; line = %i; }\n", textLen, text, t.tt, t.line);
    }
}
