#include "compiler.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <cstring>
#include <regex>

#include <iostream>
#include <fmt/core.h>
#include <tsl/ordered_map.h>

int C::compile(std::string_view input_file_name, std::string_view output_file_name)
{
    // No Optimizations
    // TODO: Optimizations

    // Key:   Function name
    // Value: Label & Input Data Registers
    std::unordered_map<std::string, function_label> functions;
    std::stringstream                               compile_result;
    auto input_file = std::ifstream(input_file_name.data());

    // Datatypes and their byte lengths
    std::unordered_map<std::string_view, int> datatypes = { { "void", 0 },
                                                            { "char", 1 },
                                                            { "short", 2 },
                                                            { "int", 4 },
                                                            { "long", 8 } };

    // Outer vector = Scopes
    // Inner map = Variables
    // Variables layout = { name, { memory_offset, datatype } }
    std::vector<tsl::ordered_map<std::string, std::pair<int, std::string>>> scopes;

    int min_regs = 0;
    int min_ram  = 0;
    int labels   = 0;

    scopes.push_back({});
    bool func = false;

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

        std::cout << line << "\n";

        for (int i = 0; i < words.size(); i++)
        {
            if (datatypes.find(words[i]) != datatypes.end())
            {
                // Function Decloration
                if ((index = words[i + 1].find('(')) != std::string_view::npos)
                {
                    auto func_name = std::string(words[++i].data(), index);
                    int  var_count;
                    if (words[i][index + 1] == ')')
                        var_count = 0;
                    else
                        var_count =
                          std::count(line.begin(), line.begin() + line.find(')'), ',') + 1;

                    min_regs = std::max(min_regs, var_count);
                    if (functions.find(func_name) != functions.end())
                    {
                        std::cerr << "ERROR: Function Redefinition at " << func_name << "\n";
                        input_file.close();
                        return -1;
                    }
                    functions.insert(std::pair<std::string, function_label>(
                      func_name,
                      { ".L" + std::to_string(labels),
                        var_count,
                        std::move(std::string(words[i - 1])) }));
                    compile_result << fmt::format(".L{} // {}\n", labels, func_name);
                    labels++;

                    scopes.push_back({});
                    func = true;

                    for (int j = 0; j < var_count; j++)
                    {
                        auto var_type = std::string(words[i + (j * 2)]);
                        auto var_name = std::string(words[i + (j * 2) + 1]);

                        // Clean up strings
                        if ((index = var_name.find(',')) != std::string::npos)
                            var_name = var_name.substr(0, index);
                        if (var_name.back() == ')')
                            var_name = var_name.substr(0, var_name.size() - 1);

                        if ((index = var_type.find(',')) != std::string::npos)
                            var_type = var_type.substr(index + 1);
                        if ((index = var_type.find('(')) != std::string::npos)
                            var_type = var_type.substr(index + 1);

                        if (datatypes.find(var_type) == datatypes.end())
                        {
                            std::cerr << "Unknown datatype " << var_type << "\n";
                            input_file.close();
                            return -1;
                        }

                        int mem_offset;

                        if (!scopes.back().empty())
                        {
                            if (scopes.back().find(var_name) != scopes.back().end())
                            {
                                std::cerr << "Variable redefinition(" << var_name
                                          << ") in same scope\n";
                                input_file.close();
                                return -1;
                            }
                            mem_offset = scopes.back().back().second.first +
                              datatypes.at(scopes.back().back().second.second);
                        }
                        else
                        {
                            if (scopes.at(scopes.size() - 2).empty())
                                mem_offset = 0;
                            else
                                mem_offset = scopes.at(scopes.size() - 2).back().second.first +
                                  datatypes.at(scopes.at(scopes.size() - 2).back().second.second);
                        }

                        std::cout << var_type << "\n";

                        min_ram = std::max(min_ram, mem_offset + datatypes.at(var_type));

                        scopes.back().insert(
                          { std::move(var_name), { mem_offset, std::move(var_type) } });
                    }
                }
                else    // Variable Decloration
                {
                    auto var_type = std::string(words[i]);
                    auto var_name = std::string(words[++i]);

                    if (var_type == "void")
                    {
                        std::cerr << "Void Variable " << var_name << "\n";
                        input_file.close();
                        return -1;
                    }

                    int mem_offset, blen;

                    blen = datatypes.at(var_type);

                    if (!scopes.back().empty())
                    {
                        if (scopes.back().find(var_name) != scopes.back().end())
                        {
                            std::cerr << "Variable redefinition(" << var_name
                                      << ") in same scope\n";
                            input_file.close();
                            return -1;
                        }
                        mem_offset = scopes.back().back().second.first +
                          datatypes.at(scopes.back().back().second.second);
                    }
                    else
                    {
                        if (scopes.at(scopes.size() - 2).empty())
                            mem_offset = 0;
                        else
                            mem_offset = scopes.at(scopes.size() - 2).back().second.first +
                              datatypes.at(scopes.at(scopes.size() - 2).back().second.second);
                    }

                    min_ram = std::max(min_ram, mem_offset + blen);

                    scopes.back().insert({ var_name, { mem_offset, var_type } });

                    // Initalize Variable
                    if (words.size() - i > 2 && words[++i][0] == '=')
                    {
                        if (std::isdigit(words[++i][0]))
                        {
                            unsigned int init_val;
                            if (std::strncmp(words[i].substr(0, 2).data(), "0x", 2) == 0)
                                init_val = std::stoul(words[i].data(), nullptr, 16);
                            else if (std::strncmp(words[i].substr(0, 2).data(), "0b", 2) == 0)
                                init_val = std::stoul(words[i].data(), nullptr, 2);
                            else
                                init_val = std::stoul(words[i].data(), nullptr, 10);

                            compile_result << "// " << var_name << "\n";

                            if (blen > 0)
                                for (int j = 0; j < blen; j++)
                                    compile_result << fmt::format(
                                      "STORE {}, {}\n",
                                      mem_offset + j,
                                      (init_val >> j * 8) & 0xff);
                        }
                        else
                        {
                            // TODO: Function Variable initalizing
                        }
                    }
                }
            }
            if (words[i].find('}') != std::string_view::npos && func)
            {
                compile_result << "RET\n\n";
                scopes.pop_back();
                func = false;
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
      "BITS >= 8\nMINREG {}\nMINRAM {}\n\nCAL {}\n\n",
      min_regs,
      min_ram,
      functions["main"].label);
    output_file << compile_result.str();

    output_file.flush();
    output_file.close();

    return 0;
}