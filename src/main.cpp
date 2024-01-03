#include <iostream>
#include <stdexcept>

#include "MemoryContext.h"
#include "Parser.h"
#include "VM.h"

void parse_options(int argc, char *argv[]) {

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.length() < 2 || arg.substr(0, 2) != "--") { throw std::invalid_argument("Can't parse argument " + arg); }
        std::string rest = arg.substr(2);

        std::vector<std::string> split;
        {
            std::istringstream ins(rest);
            std::string cur;
            while (std::getline(ins, cur, ' ')) { split.emplace_back(cur); }
        }

        if (split.empty()) throw std::invalid_argument("Can't parse argument " + arg);

        if (split.at(0) == "log") {
            if (split.size() != 3) throw std::invalid_argument("Log options must be in format --log TAG LEVEL, instead have: " + arg);
            try {
                Logger::set_level(split.at(1), std::stoi(split.at(2)));
            } catch (...) { throw std::invalid_argument("Log options must be in format --log TAG LEVEL, instead have: " + arg); }
        } else if (split.size() == 1) {
            std::string str = split.at(0);
            if (str.back() != '+' || str.back() != '-') {
                throw std::invalid_argument("Bool options must be in format --option[+/-], instead have" + arg);
            }
            Options::set_bool(str.substr(0, str.length() - 1), str.back() == '+' ? true : false);
        } else if (split.size() == 2) {
            try {
                Options::set_int(split.at(0), std::stoi(split.at(1)));
            } catch (...) { throw std::invalid_argument("Log options must be in format --log TAG LEVEL, instead have: " + arg); }
        } else {
            throw std::invalid_argument("Can't parse argument " + arg);
        }
    }
}

int main(int argc, char *argv[]) {
    try {
        Options::reset();
        Logger::reset();

        parse_options(argc, argv);

        Handle repl;
        {
            Parser parser;
            parser.loadStr("(READ EVAL PRINT STOP)");
            repl = parser.parseExpr();
        }
        VM vm;

        while (true) {
            std::cout << std::endl;
            vm.loadControl(repl);
            vm.run();
            std::cout << std::endl;
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
