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

#include "expression_parser.hpp"

#include "error_reporting.hpp"

namespace reqvm {

expression_parser::expression_parser(
    const std::string& source,
    const std::unordered_map<std::string, macro>& macros) {
    auto tokens = lex(source);
    if (tokens.empty()) {
        return;
    }
    parse(tokens, macros);
}

auto expression_parser::lex(const std::string& source) -> std::vector<token> {
    std::vector<token> tokens;

    for (std::size_t i = 0; i < source.size(); i++) {
        if (std::isspace(source[i])) {
            continue;
        }

        // Skip new line escapes
        if (source[i] == '\\') {
            continue;
        }

        token current_token;

        if (std::isdigit(source[i])) {
            current_token.type = token::type::number;

            auto number_end = i;
            while (not std::isspace(source[number_end])) {
                number_end++;
            }
            current_token.source = {source.data() + i, number_end - i};
            tokens.push_back(current_token);
            continue;
        }

        if (source[i] == '@') {
            current_token.type = token::type::macro;

            auto macro_name_end = i + 1;
            while (not std::isspace(source[macro_name_end])) {
                macro_name_end++;
            }

            if (macro_name_end == i + 1) {
                report_to_user(level::error, "Stray '@' in expression");
                return {};
            }

            current_token.source = {source.data() + i + 1,
                                    macro_name_end - i + 1};
            tokens.push_back(current_token);
            continue;
        }

        auto is_single_char_operator = [](char c) {
            return c == '+' || c == '-' || c == '%' || c == '/' || c == '*'
                   || c == '~' || c == '!';
        };
        if (is_single_char_operator(source[i])) {
            current_token.type == token::type::operator_;
            current_token.source = {source.data() + i, 1};
            tokens.push_back(current_token);
            continue;
        }

        if (source[i] == '&' || source[i] == '|') {
            if (source.size() - i < 2) {
                report_to_user(level::error,
                               "Invalid operator at end of expression");
                return {};
            }
            if (source[i + 1] == source[i]) {
                current_token.type   = token::type::operator_;
                current_token.source = {source.data() + i, 2};
                tokens.push_back(current_token);
                continue;
            }
            current_token.type   = token::type::operator_;
            current_token.source = {source.data() + i, 1};
            tokens.push_back(current_token);
            continue;
        }

        report_to_user(level::error, "Invalid character in expression");
        return {};
    }
    return tokens;
}

using parse_function = void (*)()

}   // namespace reqvm
