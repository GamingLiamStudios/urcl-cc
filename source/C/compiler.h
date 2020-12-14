#pragma once

#include <string_view>
#include <string>

namespace C
{
    struct function_label
    {
        std::string label;
        int         var_count;
        std::string return_type;
    };

    int compile(std::string_view input_file_name, std::string_view output_file_name);
}    // namespace C