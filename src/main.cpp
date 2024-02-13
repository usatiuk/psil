#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>

#include "MemoryContext.h"
#include "Parser.h"
#include "VM.h"

std::optional<std::string> infile;

void parse_options(int argc, char *argv[]) {

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-f") {
            if (++i >= argc) throw std::invalid_argument("-f given but no file");
            infile = argv[i];
            continue;
        }

        if (arg.length() < 2 || arg.substr(0, 2) != "--") {
            throw std::invalid_argument("Can't parse argument " + arg);
        }
        std::string rest = arg.substr(2);

        std::vector<std::string> split;
        {
            std::istringstream ins(rest);
            std::string cur;
            while (std::getline(ins, cur, ':')) { split.emplace_back(cur); }
        }

        if (split.empty()) throw std::invalid_argument("Can't parse argument " + arg);

        if (split.at(0) == "log") {
            if (split.size() != 3) throw std::invalid_argument("Log options must be in format --log:TAG:LEVEL");
            try {
                Logger::set_level(Logger::str_to_tag(split.at(1)), std::stoi(split.at(2)));
            } catch (...) { throw std::invalid_argument("Log options must be in format --log:TAG:LEVEL"); }
        } else if (split.size() == 1) {
            std::string str = split.at(0);
            if (str.back() != '+' && str.back() != '-') {
                throw std::invalid_argument("Bool options must be in format --option[+/-], instead have" + arg);
            }
            Options::set<bool>(str.substr(0, str.length() - 1), str.back() == '+' ? true : false);
        } else if (split.size() == 2) {
            try {
                Options::set<size_t>(split.at(0), std::stoi(split.at(1)));
            } catch (...) { throw std::invalid_argument("Options must be in format --OPTION:VALUE"); }
        } else {
            throw std::invalid_argument("Can't parse argument " + arg);
        }
    }
}

int main(int argc, char *argv[]) {
    try {
        // Just to be safe initialize everything in proper order
        Options::reset();
        Logger::reset();
        MemoryContext::get();

        parse_options(argc, argv);

        Handle repl = Parser::parse_str("(READ EVAL PRINT STOP)");
        Handle epl = Parser::parse_str("(EVAL PRINT STOP)");

        VM vm;

        if (infile) {
            Handle parsed;

            std::stringstream buffer;
            buffer << "(";
            {
                std::ifstream t(*infile);
                if (!t.is_open()) throw std::invalid_argument("Requested file could not be opened");
                buffer << t.rdbuf();
            }
            buffer << ")";

            parsed = Parser::parse_str(buffer.str());

            Handle cur_expr = parsed;
            while (!cur_expr.null()) {
                vm.loadStack(Handle::cons(cur_expr.car(), nullptr));
                vm.loadControl(epl);
                vm.run();
                cur_expr = cur_expr.cdr();
            }
        }

        if (Options::get<bool>("repl"))
            while (true && !vm.is_quit()) {
                std::cout << "> ";
                vm.loadControl(repl);
                vm.run();
            }

    } catch (const std::exception &e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Crash!" << std::endl;
        return -1;
    }
    return 0;
}
