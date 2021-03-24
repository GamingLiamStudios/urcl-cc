#pragma once

#include <exception>

class TokenizationError : std::exception
{
public:
    TokenizationError(const size_t line, const std::string reason)
        : _reason(reason), _line(line) { }
    const char * what() { return _reason.c_str(); }
    const size_t line() { return _line; }

private:
    const std::string _reason;
    const size_t      _line;
};