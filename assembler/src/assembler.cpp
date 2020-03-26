#include "assembler.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <unordered_map>
#include <string>

// This is an incredibly dumb hashing functions, we just make the bytes 
// of the string be a std::uint64_t instead.
// Not cryptographically secure, but we don't need that.
// A current limitation is that an instruction can't be more than 8 characters
// long, but who needs that anyway?
static constexpr auto hash(const char* str, std::size_t len) -> std::uint64_t {
    if (len > 9) {
        return ~0ull;
    }

    char s[8] = {0};
    for (auto i = std::size_t{0}; i < len; i++) {
        s[i] = str[i];
    }

    return static_cast<std::uint64_t>(s[0]) << 56
        | static_cast<std::uint64_t>(s[1]) << 48
        | static_cast<std::uint64_t>(s[2]) << 40
        | static_cast<std::uint64_t>(s[3]) << 32
        | static_cast<std::uint64_t>(s[4]) << 24
        | static_cast<std::uint64_t>(s[5]) << 16
        | static_cast<std::uint64_t>(s[6]) << 8
        | static_cast<std::uint64_t>(s[7]);

}

static constexpr auto operator""_u64(const char* s, std::size_t l) 
    -> std::uint64_t {
    return hash(s, l);
}

namespace reqvm {

auto assembler::run() -> int {
    int error = 0; // preprocess();
    if (error) {
        return error;
    }
    return assemble();
}
/*
auto assembler::preprocess() -> int {
    std::unordered_map<std::string, std::string> macros;

    preprocessed_file = file_name + "reqvm_pp_no_steal_2020.reqvm";
    std::ifstream in {file_name};
    std::ofstream out {preprocessed_file};

    auto pp_if = [&macros](const std::string& line, bool is_elif) -> bool {
        std::uint64_t last_integer;
        bool expression_val = true;
        std::size_t idx = is_elif ? 6 : 4; // skip %if/%elif and the first space
        while (idx < line.size()) {
            if (std::isdigit(line[idx])) {
                std::string num;
                while (line[idx] != ' ') {
                    if (!isdigit(line[idx])) {
                        // TODO: spew error
                        break;
                    }
                    num.push_back(line[idx]);
                }

            } else if (std::isalpha(line[idx])) {
                // spew an error, quit
            } else {
                switch (line[idx]) {
                case '@': {
                    // expand macro
                    break;
                }
                case '&': {
                    // do LOGICAL AND sjht
                }
                case '|': {
                    // do LOGICAL OR shit
                }
                case '>': {
                    //  do greater than, maybe abstract this
                    // in a lambda so i can do code reuse
                }
                case '<': {
                    // less than
                }
                case '=': {
                    // equals
                }
                default: {
                    // error out probably?
                }
                };
            }
            if (!expression_val) {
                return false;
            }
        }
        return true;
    };
    std::string line;
    while (std::getline(in, line)) {
        if (line[0] == '%') {
            std::string directive;
            std::size_t i;
            for (i = 1; i < line.size() && line[i] != ' '; ++i) {
                directive.push_back(line[i]);
            }
            if (directive == "define") {
                i++; // skip a space
                std::string name;
                for (; i < line.size() && line[i] != ' '; i++) {
                    name.push_back(line[i]);
                }
                std::string body;
                for (; i < line.size(); i++) {
                    body.push_back(line[i]);
                }
                std::string macro_line;
                while (std::getline(in, macro_line)) {
                    body += ("\n" + macro_line);
                }
                macros[name] = body;
            } else if (directive == "if") {
                auto expr = pp_if(line, false);
                while (line.find_first_of("%elif") != std::string::npos
                    || line.find_first_of("%else") != std::string::npos
                    || line.find_first_of("%end") != std::string::npos) {
                    std::getline(in, line);
                }
            } else if (directive == "else") {
                // report a mismatched thing
            } else if (directive == "elif") {
                // mismatch!
            } else if (directive == "end") {
                // likely this will report a mismatched end
            }

        }
    }

}*/

auto assembler::assemble() -> int {
    std::ifstream file {preprocessed_file};

    std::string line;
    while (std::getline(file, line)) {
        switch (line[0]) {
            case '.':
                process_label(line);
                break;
            case '\t':
                process_instruction(line);
                break;
            case '%':
                _has_errors = true;
                // error about running the (currently nonexistant) preprocessor first
                break;
            case ';':
                // comments get ignored
                break;
            default:
                // generic error i guess
                break;
        }
        
    }
    if (_has_errors) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

auto assembler::process_instruction(const std::string& line) -> void {
    std::size_t idx = 0;
    
    while (line[idx] != ' ') {
        idx++;
    }
    // We can handle multiple opcodes with the same amount of operands at 
    // the same time 
    using common::opcode;
    switch (auto op = get_opcode(line, idx); op) {
    case opcode::noop:
    case opcode::ret:
    case opcode::halt:
        emit_op(op);
        break;

    // All the one operand opcodes
    case opcode::push: {
        auto r1 = get_register(line, idx, false);
        emit_op(op, r1);
        break;
    }
    case opcode::pushc: {
        emit_pushc(line);
        break;
    }
    case opcode::pop: {
        auto r1 = get_register(line, idx, true);
        emit_op(op, r1);
        break;
    }

    // All the two operand opcodes
    case opcode::add:
    case opcode::sub:
    case opcode::mul:
    case opcode::div:
    case opcode::mod:
    case opcode::and_:
    case opcode::or_:
    case opcode::xor_:
    case opcode::lshft:
    case opcode::rshft:
    case opcode::cmp: {
        auto r1 = get_register(line, idx, true);
        auto r2 = get_register(line, idx, false);
        emit_op(op, r1, r2);
        break;
    }

    // All the jumps
    case opcode::call: // a call is just a fancy jump
    case opcode::jmp:
    case opcode::jeq:
    case opcode::jneq:
    case opcode::jl:
    case opcode::jleq:
    case opcode::jg:
    case opcode::jgeq: {
        std::size_t idx = line.find_last_of(' ');
        idx++;
        emit_jump(line, idx);
        break;
    }

    default:
        // TODO: spew error better
        throw 0;
    }

}

auto assembler::get_opcode(const std::string& line, std::size_t len) 
    -> common::opcode {
    using common::opcode;

    switch (hash(line.c_str(), len)) {
    case "noop"_u64:
        return opcode::noop;
    case "call"_u64: 
        return opcode::call;
    case "ret"_u64:
        return opcode::ret;
    case "add"_u64:
        return opcode::add;
    case "sub"_u64:
        return opcode::sub;
    case "mul"_u64:
        return opcode::mul;
    case "div"_u64:
        return opcode::div;
    case "mod"_u64:
        return opcode::mul;
    case "and"_u64:
        return opcode::and_;
    case "or"_u64:
        return opcode::or_;
    case "xor"_u64:
        return opcode::xor_;
    case "not"_u64:
        return opcode::not_;
    case "lshft"_u64:
        return opcode::lshft;
    case "rshft"_u64:
        return opcode::rshft;
    case "push"_u64:
        return opcode::push;
    case "pushc"_u64:
        return opcode::pushc;
    case "pop"_u64:
        return opcode::pop;
    case "cmp"_u64:
        return opcode::cmp;
    case "jmp"_u64:
        return opcode::jmp;
    case "jeq"_u64:
        return opcode::jeq;
    case "jneq"_u64:
        return opcode::jneq;
    case "jl"_u64:
        return opcode::jl;
    case "jleq"_u64:
        return opcode::jleq;
    case "jg"_u64:
        return opcode::jg;
    case "jgeq"_u64:
        return opcode::jgeq;
    case "halt"_u64:
        return opcode::halt;
    default:
        // TODO: throw an actually informative exception
        throw 0;
    }

}

auto assembler::get_register(
    const std::string& line,
    std::size_t start,
    bool is_lhs
) -> common::registers {
    std::size_t idx = start;
    while (line[idx] != ' ' && line[idx] != ',') {
        idx++;
    }

#define CASE_GP(n)           \
    case "gp" #n##_u64:       \
        return common::registers::gp##n;
#define CASE_IFA(n)                     \
    case "ifa" #n##_u64:                 \
        return common::registers::ifa##n;

    auto the_register = std::string{line.begin() + start, line.begin() + idx};
    switch (hash(the_register.c_str(), idx - start)) {
    case "sp"_u64:
        if (is_lhs) {
            // error
        }
        return common::registers::sp;
    case "pc"_u64:
        if (is_lhs) {
            // error
        }
        return common::registers::pc;
    case "ire"_u64:
        return common::registers::ire;
    CASE_GP(00)
    CASE_GP(01)
    CASE_GP(02)
    CASE_GP(03)
    CASE_GP(04)
    CASE_GP(05)
    CASE_GP(06)
    CASE_GP(07)
    CASE_GP(08)
    CASE_GP(09)
    CASE_GP(10)
    CASE_GP(11)
    CASE_GP(12)
    CASE_GP(13)
    CASE_GP(14)
    CASE_GP(15)
    CASE_GP(16)
    CASE_GP(17)
    CASE_GP(18)
    CASE_GP(19)
    CASE_GP(20)
    CASE_GP(21)
    CASE_GP(22)
    CASE_GP(23)
    CASE_GP(24)
    CASE_GP(25)
    CASE_GP(26)
    CASE_GP(27)
    CASE_GP(28)
    CASE_GP(29)
    CASE_GP(30)
    CASE_GP(31)
    CASE_GP(32)
    CASE_GP(33)
    CASE_GP(34)
    CASE_GP(35)
    CASE_GP(36)
    CASE_GP(37)
    CASE_GP(38)
    CASE_GP(39)
    CASE_GP(40)
    CASE_GP(41)
    CASE_GP(42)
    CASE_GP(43)
    CASE_GP(44)
    CASE_GP(45)
    CASE_GP(46)
    CASE_GP(47)
    CASE_GP(48)
    CASE_GP(49)
    CASE_GP(50)
    CASE_GP(51)
    CASE_GP(52)
    CASE_GP(53)
    CASE_GP(54)
    CASE_GP(55)
    CASE_GP(56)
    CASE_GP(57)
    CASE_GP(58)
    CASE_GP(59)
    CASE_GP(60)
    CASE_GP(61)
    CASE_GP(62)
    CASE_GP(63)
    CASE_IFA(00)
    CASE_IFA(01)
    CASE_IFA(02)
    CASE_IFA(03)
    CASE_IFA(04)
    CASE_IFA(05)
    CASE_IFA(06)
    CASE_IFA(07)
    CASE_IFA(08)
    CASE_IFA(09)
    CASE_IFA(10)
    CASE_IFA(11)
    CASE_IFA(12)
    CASE_IFA(13)
    CASE_IFA(14)
    CASE_IFA(15)
#undef CASE_GP
#undef CAsE_IFA
    default:
        return common::registers::none;
        // report an error
    }
}

auto assembler::emit_pushc(const std::string& line) -> void {
    std::string instruction;
    instruction.resize(6);
    std::copy_n(line.begin(), 5, instruction.begin());
    if (instruction != "pushc") {
        // error
    }

    std::size_t num_start = 6;
    while (!isdigit(line[num_start])) { num_start++; }

    auto num_candidate = std::string{line.begin() + num_start, line.end()};
    try {
        auto the_num = std::stoull(num_candidate, nullptr);
        _output.push_back(static_cast<std::uint8_t>(common::opcode::pushc));
        while (the_num) {
            auto hi = the_num >> 8;
            _output.push_back(static_cast<std::uint8_t>(hi));
            the_num <<= 8;
        }
        _idx += 9;
    } catch (const std::out_of_range& e) {
        // error
    }
}

auto assembler::emit_op(
    common::opcode op,
    common::registers r1,
    common::registers r2
) -> void {
    _output.push_back(static_cast<std::uint8_t>(op));
    _idx++;
    if (r1 != common::registers::none) {
        _output.push_back(static_cast<std::uint8_t>(r1));
        _idx++;
        if (r2 != common::registers::none) {
            _output.push_back(static_cast<std::uint8_t>(r2));
            _idx++;
        }
    }
} 


auto assembler::emit_jump(const std::string& line, std::size_t label_start)
    -> void {
    auto jump_kind = std::string {
        line.begin(),
        line.begin() + line.find_first_of(' ')
    };
    auto op = get_opcode(jump_kind.data(), jump_kind.size());
    _output.push_back(static_cast<std::uint8_t>(op));
    auto label = std::string{line.begin() + label_start, line.end()};
    if (_labels.find(label) == _labels.end()) {
        auto tmp = _labels[label][0];
        while (tmp) {
            auto hi = tmp >> 8;
            _output.push_back(hi);
            tmp <<= 8; 
        }
    } else {
        _labels[label] = std::vector<std::uint64_t>{};
        _labels[label].push_back(0); // this mean we jump ahead and don't have the label yet
        _labels[label].push_back(_idx);
    }
    _idx += 9;
}

auto assembler::process_label(const std::string& line) -> void {
    if (line.find_first_of(':') == std::string::npos) {
        // error
        return;
    }
    auto label_name = std::string{
        line.begin() + 1,
        line.begin() + line.find_first_of(':')
    };

    if (_labels.find(label_name) != _labels.end()) {
        // error about how label names must not be reused or smth
        return;
    }

    _labels[label_name] = std::vector<std::uint64_t>{};
    _labels[label_name].push_back(_idx);
}

}
