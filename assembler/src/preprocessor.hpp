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

#pragma once

#include "assertions.hpp"
#include "instruction.hpp"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace reqvm {

class macro {
    friend class preprocessor;

public:
    macro()             = default;
    macro(const macro&) = default;
    macro(macro&&)      = default;

    macro& operator=(const macro&) = default;
    macro& operator=(macro&) = default;

    ~macro() noexcept = default;

    enum class type {
        integral,
        string,
        instruction_list,
    };

    type type_of_self() { return _type; }

    auto as_integral() {
        ASSERT(_type == type::integral);
        return _integral_value;
    }
    auto as_string() -> std::string& {
        ASSERT(_type == type::string);
        return _string_value;
    }
    auto as_instruction_list() -> std::vector<instruction>& {
        ASSERT(_type == type::instruction_list);
        return _instruction_list;
    }

private:
    macro(type t) : _type {t} {}

    type _type;

    std::uint64_t _integral_value;
    std::string _string_value;
    std::vector<instruction> _instruction_list;
};

class preprocessor {
public:
    explicit preprocessor(std::string&& source);
    ~preprocessor() noexcept = default;

private:
    enum class keyword {
        invalid,
        if_,
        else_,
        elif,
        end,
        define,
    };
    static auto get_keyword_from(const std::string& line) -> keyword;

    bool _had_errors {false};
    macro _result;
};

}   // namespace reqvm
