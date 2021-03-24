#include "compiler.h"

#include <iostream>

Compiler::Compiler(std::vector<Token> &tokens)
{
    size_t index = 0;

    while (index < tokens.size())
    {
        auto &token = tokens[index++];

        switch (token.type)
        {
        case Token::Types::Keyword: std::cout << "Keyword: " << token.val << "\n"; break;
        case Token::Types::Identifier: std::cout << "Identifier: " << token.val << "\n"; break;
        case Token::Types::Constant: std::cout << "Constant: " << token.val << "\n"; break;
        case Token::Types::Operator: std::cout << "Operator: " << token.val << "\n"; break;
        case Token::Types::Separator: std::cout << "Separator: " << token.val << "\n"; break;
        }
    }
}

void Compiler::write_to_file()
{
}
