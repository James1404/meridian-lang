#include "meridian.h"
#include "meridian_ast.h"
#include "meridian_error.h"
#include "meridian_lex.h"
#include "meridian_parser.h"
#include "meridian_tokens.h"
#include "meridian_string.h"
#include "meridian_types.h"

#include <stdio.h>
#include <stdlib.h>

void Meridian_run(String src) {
    TokenList tokens = TokenList_make(src);
    
    Lexer lexer = Lexer_new(&tokens);
    Lexer_run(&lexer);

    if(Meridian_error_found()) return;

    ASTList initialAST = ASTList_make();

    Parser parser = Parser_make(&tokens, &initialAST);
    Parser_run(&parser);

    if(Meridian_error_found()) return;

    RunTypeChecker(&initialAST, parser.root);

    ASTList_prettyPrint(&initialAST, 0, parser.root);

    Parser_free(&parser);
    ASTList_free(&initialAST);
    Lexer_free(&lexer);
    TokenList_free(&tokens);
}

void Meridian_run_file(String filepath) {
    String nullterminated;
    STR_CPY_ALLOC_NULL(nullterminated, filepath);
    
    FILE* file = fopen(nullterminated.raw, "r");

    if(!file) {
        Meridian_error("Could not find file '%.*s'", filepath.len, filepath.raw);
        return;
    }

    fseek(file, 0, SEEK_END);
    u64 len = ftell(file);

    fseek(file, 0, SEEK_SET);
    char* buffer = malloc(len);

    if(fread(buffer, 1, len, file) != len) {
        Meridian_error("Failed to read file '%.*s'", filepath.len, filepath.raw);
        return;
    }
    fclose(file);

    Meridian_run((String) { buffer, len });
}
