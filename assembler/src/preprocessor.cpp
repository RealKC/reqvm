/*
 * MIT License
 *
 * Copyright (c) 2020 Mitca Dumitru
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "preprocessor.hpp"

#include <cctype>
#include <sstream>
#include <string_view>
#include <unordered_map>

namespace reqvm {

preprocessor::preprocessor(std::string&& source_str) {
    std::istringstream source {std::move(source_str)};

    bool is_in_instruction_list {false};
    bool may_get_else_or_elif_directive {false};
    bool line_was_consumed_already {false};
    bool done {false};

    std::string line;
    while (not done) {
        if (not line_was_consumed_already) {
            if (not std::getline(source, line)) {
                _had_errors = true;
                break;
            }
        }
        if (line[0] == '%') {
            switch (get_keyword_from(line)) {
            case keyword::if_:
                may_get_else_or_elif_directive = true;
                // TODO
                break;
            case keyword::else_:
                if (not may_get_else_or_elif_directive) {
                    // Error
                }
                may_get_else_or_elif_directive = false;
                // TODO
                break;
            case keyword::elif:
                may_get_else_or_elif_directive = true;
                // TODO
                break;
            case keyword::define: {
                macro m {extract_macro_type(line)};
                break;
            }
            case keyword::end:
                done = true;
                // TODO
                break;
            case keyword::invalid:
                // TODO
                break;
            }
        } else if (line[0] == '\t') {
            // TODO
        } else {
            // TODO
        }
    }
}

auto preprocessor::get_keyword_from(const std::string& line) -> keyword {
    ASSERTM(line[0] == '%', "get_keyword_from should not be used to extract a "
                            "keyword from a line not starting with '%'");

    if (line.length() < (sizeof("%define") - 1)) {
        return keyword::invalid;
    }

    static std::unordered_map<std::string_view, keyword> keywords {
        {"if", keyword::if_},
        {"elif", keyword::elif},
        {"else", keyword::else_},
        {"end", keyword::end},
        {"define", keyword::define}};

    std::string_view attempt {line.data() + 1, line.find_first_of(' ')};
    return keywords.find(attempt) != keywords.end() ? keywords[attempt]
                                                    : keyword::invalid;
}

auto preprocessor::extract_macro_type(const std::string& line) -> macro::type {
    auto type_start = line.find_first_of('%', 1) + 1;   // Skips leading '%'

    if (std::string_view {line.data() + type_start, 4} != "type") {
        _had_errors = true;
        return macro::type::invalid;
    }

    type_start += 4;

    while (std::isspace(line[type_start])) {
        type_start++;
    }

    static std::unordered_map<std::string_view, macro::type> macro_types {
        {"integral", macro::type::integral},
        {"string", macro::type::string},
        {"instruction_list", macro::type::instruction_list}};

    auto next_space = line.find_first_of(' ', type_start);
    std::string_view attempt {line.data() + type_start,
                              next_space - type_start};

    return macro_types.find(attempt) != macro_types.end()
               ? macro_types[attempt]
               : macro::type::invalid;
}

}   // namespace reqvm
