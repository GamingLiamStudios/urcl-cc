#pragma once

#include <string>
#include <string_view>
#include <vector>

struct Token
{
    enum class Types
    {
        Keyword,
        Identifier,
        Constant,
        Operator,
        Separator
    };

    const Types       type;
    const std::string val;
};

std::vector<Token> tokenize(std::string_view);
