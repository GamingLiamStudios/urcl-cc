#include <iostream>
#include <fstream>

#include <fmt/format.h>

#include "token.h"
#include "compiler.h"
#include "util/error.h"

int main()
{
    std::string   text;
    std::ifstream file;
    file.open("test.c", std::ios::in | std::ios::ate);
    text.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&text[0], text.size());
    file.close();

    try
    {
        auto tokens = tokenize(text);
        // auto compiler = Compiler(tokens);

        for (auto &token : tokens)
        {
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
    catch (TokenizationError &e)
    {
        std::cerr << fmt::format("Error at line: {}\n\twhat: {}\n", e.line(), e.what());
        return -1;
    }
}
