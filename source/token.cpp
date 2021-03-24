#include "token.h"

#include <iostream>
#include <set>
#include <cctype>

#include "util/types.h"
#include "util/error.h"

namespace
{
    std::set<std::string> keywords   = { "auto",
                                       "break",
                                       "case",
                                       "char",
                                       "const",
                                       "continue",
                                       "default",
                                       "do",
                                       "double",
                                       "else",
                                       "enum",
                                       "extern"
                                       "float",
                                       "for",
                                       "goto",
                                       "if",
                                       "int",
                                       "long",
                                       "register",
                                       "return",
                                       "short",
                                       "signed",
                                       "sizeof",
                                       "static"
                                       "struct",
                                       "switch",
                                       "typedef",
                                       "union",
                                       "unsigned",
                                       "void",
                                       "volatile",
                                       "while" };
    std::set<char>        seperators = { '(', ')', '[', ']', '{', '}', ';', ',', '.', ':' };
}    // namespace

std::vector<Token> tokenize(std::string_view doc)
{
    std::vector<Token> tokens;

    auto push_token = [&](std::string &token) {
        if (token.empty() || token.front() == '\0') return;
        if (std::isdigit(token.front()))
            tokens.push_back({ Token::Types::Constant, token });
        else if (!std::isalpha(token.front()))
            tokens.push_back({ Token::Types::Operator, token });
        else if (keywords.find(token) != keywords.end())
            tokens.push_back({ Token::Types::Keyword, token });
        else
            tokens.push_back({ Token::Types::Identifier, token });
        token = "";
    };

    std::string token;
    size_t      index = 0;
    size_t      line  = 1;
    char        next;

    char comment = 0;

    while (index < doc.size())
    {
        next = doc[index++];
        /*
        TODO
        Operators
        Constants
            - Real Number(Decimal)
            - String
        */

        // Comments
        if (next == '/')
        {
            if (doc[index] == '/')
            {
                push_token(token);
                index++;
                token += "//";
                continue;
            }
            else if (doc[index] == '*')
            {
                push_token(token);
                index++;
                token += "/*";
                continue;
            }
        }
        else if (next == '*' && doc[index] == '/')
        {
            index++;
            token.clear();
            continue;
        }

        // Whitespace & line-break handling
        if (std::isspace(next) || (next == '\\' && (!token.empty() && token.front() == '\'')))
        {
            if (next == '\n')
            {
                line++;
                if (token == "//") token.clear();
            }
            else if (token == "//" || token == "/*")
                continue;
            else if (next == '\\')
            {
                while (next != '\n')
                    if (!std::isspace(next = doc[index++]))
                        throw TokenizationError(line, "Backslash in invalid location");
                line++;
            }
            else if (!token.empty())
                push_token(token);
            continue;
        }
        if (token == "//" || token == "/*") continue;

        // Character Constant
        if (next == '\'' || (!token.empty() && token.front() == '\''))
        {
            if (token.empty()) continue;
            if (token.back() == '\\')
            {
                token.back() = '\'';
                continue;
            }

            u32    res      = 0;
            size_t es_index = 0;
            for (int i = 0; i < 4; i++)
            {
                if (es_index >= token.size()) break;

                next = token.at(es_index++);
                if (next == '\\')
                {
                    next = token.at(es_index++);
                    switch (next)
                    {
                    case 'a': next = '\a'; break;
                    case 'b': next = '\b'; break;
                    case 't': next = '\t'; break;
                    case 'n': next = '\n'; break;
                    case 'v': next = '\v'; break;
                    case 'f': next = '\f'; break;
                    case 'r': next = '\r'; break;
                    case 'x':
                    {
                        // Hex
                        std::string hex_string = "";
                        while (std::isdigit(next) ||
                               (next >= 'A' || next <= 'F') && es_index < token.size())
                            hex_string += token[es_index++];

                        if (hex_string.empty())
                            throw TokenizationError(line, "Hex escape code without hex digits");

                        if (hex_string.size() > 2)
                            std::cout << "Warning: Hex escape code outside of valid ASCII "
                                         "Range(00-FF) at line "
                                      << line << "\n";

                        if (std::isdigit(hex_string[0]))
                            next = hex_string[0] - '0';
                        else
                            next = 0xA + hex_string[0] - 'A';

                        if (hex_string.size() > 1)
                            if (std::isdigit(hex_string[1]))
                                next = (next << 4) | hex_string[0] - '0';
                            else
                                next = (next << 4) | (0xA + hex_string[1] - 'A');
                    }
                    break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    {
                        // Octal
                        int octal_len = (token.at(es_index++) >= '0' || token.at(es_index++) <= '7')
                          ? ((token.at(es_index++) >= '0' || token.at(es_index++) <= '7') ? 3 : 2)
                          : 1;

                        if (
                          octal_len == 3 && token.at(es_index - 3) == '3' &&
                          token.at(es_index - 2) == '7')
                            octal_len--;

                        next = std::stoi(token.substr(es_index - octal_len, octal_len), nullptr, 8);
                    }
                    break;
                    default:
                        // Throw Error
                        throw TokenizationError(line, "Invalid escape code");
                    }
                }
                res = (res << 8) | (u8) next;
            }

            token = std::to_string(res);
            push_token(token);
            continue;
        }

        if (!std::isalnum(next))
        {
            if (seperators.find(next) != seperators.end())    // Seperator
            {
                if (!token.empty()) push_token(token);
                token += next;
                push_token(token);
                continue;
            }
            else if (!std::isdigit(token.front()) && next != '_')
                push_token(token);    // Identifier
            else if (token.front() == '0' && token.size() > 1)
            {
                // TODO: Parse hex
                push_token(token);    // Hex
            }
            else if (std::isdigit(token.front()))
                push_token(token);    // Decimal
        }
        else if (
          token.front() == '0' && token.size() > 1 && std::tolower(token.at(1)) != 'x' &&
          (next < '0' || next > '7'))
            push_token(token);    // Octal

        token += next;
    }
    if (!token.empty()) push_token(token);

    return tokens;
}
