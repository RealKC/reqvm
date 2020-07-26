/* 
 *  MIT License
 *  
 *  Copyright (c) 2020 Mitca Dumitru
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "stack.hpp"

namespace reqvm {

auto stack::push(std::uint64_t val, registers& regs) -> void {
    if (regs.sp() + 1 > (1024 * 1024)) {
        throw stack_error {
            "Stack overflow: the binary has tried writing past the end of the "
            "stack."};
    }
    _storage[regs.sp()++] = val;
}

auto stack::pop(registers& regs) -> std::uint64_t {
    if (regs.sp() == 0) {
        throw stack_error {
            "Stack underflow: the binary has tried reading before the start "
            "of the stack."};
    }
    regs.sp()--;
    return _storage[regs.sp() + 1];
}

}   // namespace reqvm
