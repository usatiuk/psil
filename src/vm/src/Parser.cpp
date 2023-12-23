//
// Created by Stepan Usatiuk on 23.12.2023.
//

#include "Parser.h"

#include <ranges>
#include <stack>

#include "VM.h"

Parser::Parser(VM &vm) : _vm(vm) {}

void Parser::loadSecd(std::string_view input) {
    _tokenizer.load(input);
    std::stack<Cell *> out;

    compileBody([&out](Cell *cmd) { out.emplace(cmd); });

    while (!out.empty()) {
        this->_vm.appendCommand(out.top());
        out.pop();
    }
}

void Parser::compileBody(const std::function<void(Cell *)> &sink) {
    auto token = _tokenizer.getNext();
    if (token != "(") throw std::invalid_argument("Expected (");

    while (token != ")") {
        token = _tokenizer.getNext();

        if (token == "NIL") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::NIL));
        } else if (token == "LDC") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
            sink(_vm.makeCell<IntCell>(std::stoi(_tokenizer.getNext())));
        } else if (token == "LD") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));
            if (_tokenizer.getNext() != "(") throw std::invalid_argument("Expected (");
            int64_t frame = std::stoi(_tokenizer.getNext());
            if (_tokenizer.getNext() != ".") throw std::invalid_argument("Expected .");
            int64_t loc = std::stoi(_tokenizer.getNext());
            if (_tokenizer.getNext() != ")") throw std::invalid_argument("Expected )");
            sink(_vm.makeCell<ConsCell>(_vm.makeCell<IntCell>(frame), _vm.makeCell<IntCell>(loc)));
        } else if (token == "SEL") {
            std::stack<Cell *> outt;
            compileBody([&outt](Cell *cmd) { outt.emplace(cmd); });
            std::stack<Cell *> outf;
            compileBody([&outf](Cell *cmd) { outf.emplace(cmd); });

            if (outt.empty()) throw std::invalid_argument("Function body empty");
            if (outf.empty()) throw std::invalid_argument("Function body empty");

            ConsCell *ttop = _vm.makeCell<ConsCell>(outt.top());
            outt.pop();
            while (!outt.empty()) {
                _vm.push(ttop, outt.top());
                outt.pop();
            }

            ConsCell *ftop = _vm.makeCell<ConsCell>(outf.top());
            outf.pop();
            while (!outf.empty()) {
                _vm.push(ftop, outf.top());
                outf.pop();
            }

            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::SEL));
            sink(ttop);
            sink(ftop);
        } else if (token == "JOIN") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::JOIN));
        } else if (token == "LDF") {
            std::stack<Cell *> out;

            compileBody([&out](Cell *cmd) { out.emplace(cmd); });

            if (out.empty()) throw std::invalid_argument("Function body empty");

            ConsCell *fntop = _vm.makeCell<ConsCell>(out.top());
            out.pop();

            while (!out.empty()) {
                _vm.push(fntop, out.top());
                out.pop();
            }
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::LDF));
            sink(fntop);
        } else if (token == "AP") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::AP));
        } else if (token == "RET") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::RET));
        } else if (token == "DUM") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::DUM));
        } else if (token == "RAP") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::RAP));
        } else if (token == "STOP") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::STOP));
        } else if (token == "ADD") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::ADD));
        } else if (token == "SUB") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::SUB));
        } else if (token == "READCHAR") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::READCHAR));
        } else if (token == "PUTCHAR") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        } else if (token == "PUTNUM") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTNUM));
        } else if (token == "CONS") {
            sink(_vm.makeCell<CommandCell>(CommandCell::CommandNum::CONS));
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
    for (const auto &w: input
                        | std::views::split(' ')
                        | std::views::transform([](auto &&rng) {
        return std::string_view(&*rng.begin(), std::ranges::distance(rng));
    })) {
        _tokens.emplace(w);
    }
}

bool Parser::Tokenizer::empty() const {
    return _tokens.empty();
}

