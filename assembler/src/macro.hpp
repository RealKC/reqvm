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
#include <unordered_map>
#include <variant>
#include <vector>

namespace reqvm {

class macro {
public:
    enum class type {
        invalid,
        integral,
        string,
        instruction_list,
        identifier,
    };

    class value {
    public:
        value(type t) : _type {t} {}
        ~value() noexcept = default;

        auto as_integral() noexcept -> std::uint64_t {
            ASSERT(_type == type::integral);
            return std::get<std::uint64_t>(_storage);
        }
        auto as_string() noexcept -> const std::string& {
            ASSERT(_type == type::string);
            return std::get<std::string>(_storage);
        }
        auto as_instruction_list() noexcept -> const std::vector<instruction>& {
            ASSERT(_type == type::instruction_list);
            return std::get<std::vector<instruction>>(_storage);
        }
        auto as_identifier() noexcept -> const std::string& {
            ASSERT(_type == type::identifier);
            return std::get<std::string>(_storage);
        }

    private:
        type _type;
        std::variant<std::uint64_t, std::string, std::vector<instruction>>
            _storage;
    };

    auto expand(const std::unordered_map<std::string, macro>& all_macros)
        -> void {
        // FIXME: Implement me
        // FIXME: make this return a macro::value
    }

    auto my_type() noexcept { return _type; }
    auto set_type(type t) noexcept { _type = t; }

private:
    type _type;
    std::string _source;
};

}   // namespace reqvm