#pragma once

#include <vector>
#include <optional>
#include <unordered_map>

#include "token.h"
#include "util/types.h"

/* IGNORE ThIS
    Declaration
        - VarDecl	    int a
        - FuncDecl	    int func(vars)
        - TypeDecl	    typedef int i32;
        - StructDecl	struct example... [varname/s]
    Constant			int value
    Assignment		    a = value
    PrefixExp		    6*2+9 -> +9*26
    FunctionCall		func(vars)
    CastExp			    (int)value
*/

struct Expression
{
};

class Compiler
{
public:
    // Parse Tokens into more computer friendly format
    Compiler(std::vector<Token> &tokens);

    // void optimize_level1();

    void write_to_file();

private:
    std::vector<Expression> parsed_tokens;
};
