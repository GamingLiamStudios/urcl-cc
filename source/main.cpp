// STL
#include <iostream>
#include <string_view>
#include <cstring>

// Compilers
#include "C/compiler.h"

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        std::cerr << "ERROR: No Compile Options Specified!\n";
        return -1;
    }

    std::string_view output_file_name = "output.urcl";
    std::string_view input_format     = "C";
    std::string_view input_file_name;

    // Read options
    // TODO: multi-word strings
    for (int i = 1; i < argc; i++)
    {
        auto str = std::string_view(argv[i], std::strlen(argv[i]));
        if (str.at(0) == '-')
        {
            // Option
            auto opt = std::string_view(str.begin() + 1, str.length() - 1);
            if (std::strcmp(opt.data(), "o") == 0) { output_file_name = argv[++i]; }
            if (std::strcmp(opt.data(), "lang") == 0) { input_format = argv[++i]; }
        }
        else
            input_file_name = str;
    }

    // Input file check
    if (input_file_name.empty())
    {
        std::cerr << "ERROR: No Input File Specified!\n";
        return -1;
    }

    int result;

    if (std::strcmp(input_format.data(), "C") == 0)
        result = C::compile(input_file_name, output_file_name);
    else
    {
        std::cerr << "ERROR: Unknown Input Language!\n";
        return -1;
    }

    return result;
}