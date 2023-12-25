//
// Created by Stepan Usatiuk on 23.12.2023.
//

#include "Parser.h"

#include <ranges>
#include <stack>

#include "Command.h"
#include "ConsUtils.h"
#include "MemoryContext.h"
#include "VM.h"

Parser::Parser(VM &vm) : _vm(vm) {}

void Parser::loadSecd(std::string_view input) {
    _tokenizer.load(input);

    MCHandle out(ConsUtils::cons(nullptr, nullptr));

    compileBody([&out](MCHandle cmd) { ConsUtils::append(out, cmd); });
    _vm.loadControl(out);
}

void Parser::compileBody(const std::function<void(MCHandle)> &sink) {
    auto token = _tokenizer.getNext();
    if (token != "(") throw std::invalid_argument("Expected (");

    while (token != ")") {
        token = _tokenizer.getNext();

        if (token == "NIL") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::NIL)));
        } else if (token == "LDC") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
            sink(ConsUtils::makeIntCell(std::stoi(_tokenizer.getNext())));
        } else if (token == "LD") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
            if (_tokenizer.getNext() != "(") throw std::invalid_argument("Expected (");
            int64_t frame = std::stoi(_tokenizer.getNext());
            if (_tokenizer.getNext() != ".") throw std::invalid_argument("Expected .");
            int64_t loc = std::stoi(_tokenizer.getNext());
            if (_tokenizer.getNext() != ")") throw std::invalid_argument("Expected )");
            sink(ConsUtils::cons(ConsUtils::makeIntCell(frame), ConsUtils::makeIntCell(loc)));
        } else if (token == "SEL") {
            MCHandle outt(ConsUtils::cons(nullptr, nullptr));
            compileBody([&outt](MCHandle cmd) { ConsUtils::append(outt, cmd); });
            MCHandle outf(ConsUtils::cons(nullptr, nullptr));
            compileBody([&outf](MCHandle cmd) { ConsUtils::append(outf, cmd);; });

            if (ConsUtils::car(outt).get() == nullptr) throw std::invalid_argument("Function body empty");
            if (ConsUtils::cdr(outt).get() == nullptr) throw std::invalid_argument("Function body empty");

            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::SEL)));
            sink(outt);
            sink(outf);
        } else if (token == "JOIN") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::JOIN)));
        } else if (token == "LDF") {
            MCHandle outt(ConsUtils::cons(nullptr, nullptr));
            compileBody([&outt](MCHandle cmd) { ConsUtils::append(outt, cmd); });

            if (ConsUtils::car(outt).get() == nullptr) throw std::invalid_argument("Function body empty");

            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDF)));
            sink(outt);
        } else if (token == "AP") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::AP)));
        } else if (token == "RET") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::RET)));
        } else if (token == "DUM") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::DUM)));
        } else if (token == "RAP") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::RAP)));
        } else if (token == "STOP") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::STOP)));
        } else if (token == "ADD") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::ADD)));
        } else if (token == "SUB") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::SUB)));
        } else if (token == "READCHAR") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::READCHAR)));
        } else if (token == "PUTCHAR") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
        } else if (token == "PUTNUM") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTNUM)));
        } else if (token == "CONS") {
            sink(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::CONS)));
        } else {
            if (token != ")")
                throw std::invalid_argument("Unknown token " + token);
        }
    }
}

std::string Parser::Tokenizer::getNext() {
    std::string ret = std::move(_tokens.front());
    _tokens.pop();
    return ret;
}

std::string_view Parser::Tokenizer::peek() const {
    return _tokens.back();
}

void Parser::Tokenizer::load(std::string_view input) {
    for (const auto &w: input | std::views::split(' ') | std::views::transform([](auto &&rng) {
                            return std::string_view(&*rng.begin(), std::ranges::distance(rng));
                        })) {
        _tokens.emplace(w);
    }
}

bool Parser::Tokenizer::empty() const {
    return _tokens.empty();
}
