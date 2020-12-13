#include "compiler.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <cstring>
#include <regex>

#include <iostream>
#include <fmt/core.h>

int C::compile(std::string_view input_file_name, std::string_view output_file_name)
{
    // No Optimizations
    // TODO: Optimizations

    // Key:   Function name
    // Value: Label & Input Data Registers
    std::unordered_map<std::string, function_label> functions;
    std::stringstream                               compile_result;
    auto input_file = std::ifstream(input_file_name.data());

    std::string_view datatypes = "void char int ";

    int min_regs = 0;
    int min_ram  = 0;
    int labels   = 0;

    bool scope;

    std::string line;
    char        buffer[1024];
    while (input_file)
    {
        // Read up to next semi-colon
        memset(buffer, 0, 1024);
        line.clear();
        input_file.getline(buffer, 1024, ';');
        line = std::string(buffer, std::strlen(buffer));

        if (line.empty()) continue;    // Prevent 'std::out_of_range'

        // Clean-up Output
        std::replace(line.begin(), line.end(), '\n', ' ');
        std::replace(line.begin(), line.end(), '\t', ' ');
        line = std::regex_replace(line, std::regex(" {2,}"), " ");

        // Remove leading spaces
        if (line.at(0) == ' ') line = line.substr(1);

        // Split into individual words
        std::vector<std::string_view> words;
        size_t                        index = 0;
        while (index < line.size())
        {
            size_t nindex = line.find(' ', index);
            if (nindex == std::string::npos) nindex = line.size();
            auto word = std::string_view(line.data() + index, nindex - index);
            words.push_back(word);
            index = nindex + 1;
        }

        for (int i = 0; i < words.size(); i++)
        {
            if (
              datatypes.find(words[i]) != std::string_view::npos &&
              (index = words[i + 1].find('(')) != std::string_view::npos)
            {
                // Function Decloration found
                auto func_name = std::string(words[++i].data(), index);
                int  var_count;
                if (words[i][index + 1] == ')')
                    var_count = 0;
                else
                    var_count = std::count(line.begin(), line.end() - line.find(')'), ',') + 1;

                if (words[i - 1] != "void") var_count++;

                min_regs = std::max(min_regs, var_count);
                if (functions.find(func_name) != functions.end())
                {
                    std::cerr << "ERROR: Function Redefinition at " << func_name << "\n";
                    input_file.close();
                    return -1;
                }
                functions.insert(std::pair<std::string, function_label>(
                  func_name,
                  { ".L" + std::to_string(labels), var_count }));
                compile_result << fmt::format(".L{} // {}\n", labels, func_name);
                labels++;

                scope = true;
            }
            if (words[i].find('}') != std::string_view::npos && scope)
            {
                compile_result << "RET\n";
                scope = false;
            }
        }
    }
    input_file.close();

    if (functions.find("main") == functions.end())
    {
        std::cerr << "ERROR: No Entry Point(main) found!\n";
        return -1;
    }

    std::cout << "Code\n\n" << compile_result.str();

    auto output_file = std::ofstream(output_file_name.data());

    output_file << fmt::format(
      "BITS >= 8\nMINREG {}\nMINRAM {}\n\nCALL {}\n\n",
      min_regs,
      min_ram,
      functions["main"].label);
    output_file << compile_result.str();

    output_file.flush();
    output_file.close();

    return 0;
}