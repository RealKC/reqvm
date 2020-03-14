#include "assembler.hpp"

#include <cctype>
#include <fstream>
#include <unordered_map>
#include <string>

namespace reqvm {

int assembler::run() {
    int error = preprocess();
    if (error) {
        return error;
    }
    return assemble();
}

int assembler::preprocess() {
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
                // parse value 
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

}

}