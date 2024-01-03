//
// Created by Stepan Usatiuk on 23.12.2023.
//

#include "Parser.h"

#include <ranges>
#include <stack>

#include "MemoryContext.h"
#include "VM.h"

Handle Parser::parse_str(std::string_view input) {
    Parser parser;
    parser.loadStr(input);
    return parser.parseExpr();
}

void Parser::loadStr(std::string_view input) { _tokenizer.load(input); }

Handle Parser::parseExpr() {
    while (!_tokenizer.empty()) {
        std::string token = std::string(_tokenizer.peek());
        if (token == "(") {
            _tokenizer.getNext();
            Handle out(Handle::cons(nullptr, nullptr));
            while (token != ")") {
                if (token == ".") {
                    _tokenizer.getNext();

                    if (_tokenizer.peek() != ")") out.setcdr(parseExpr());

                    if (_tokenizer.getNext() != ")") throw std::invalid_argument("Missing ) after pair");

                    return out;
                }
                if (_tokenizer.peek() == ")") {
                    token = _tokenizer.peek();
                    continue;
                }
                out.append(parseExpr());
                token = _tokenizer.peek();
            }
            _tokenizer.getNext();
            return out;
        } else {
            token = _tokenizer.getNext();
            if (token.find_first_not_of("0123456789") == std::string::npos ||
                (token.length() > 1 && token.at(0) == '-' && token.find_first_not_of("0123456789", 1) == std::string::npos)) {
                CellValType val = std::stoi(token);
                return Handle::makeNumCell(val);
            } else {
                return Handle::makeStrCell(token);
            }
        }
    }

    return Handle::cons(nullptr, nullptr);
}

std::string Parser::Tokenizer::getNext() {
    std::string ret = std::move(_tokens.front());
    _tokens.pop();
    return ret;
}

std::string_view Parser::Tokenizer::peek() const { return _tokens.front(); }

void Parser::Tokenizer::load(std::string_view input) {
    std::string_view::size_type curpos = input.find_first_not_of(std::string{' ', '\n', '\r'});

    static const std::string alnum = "-+><?=!*/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static const std::string special = "().";

    while (curpos != std::string_view::npos) {
        if (alnum.find(input.at(curpos)) != std::string::npos) {
            std::string_view::size_type end = input.find_first_not_of(alnum, curpos);
            if (end == std::string_view ::npos) end = input.size();
            _tokens.emplace(input.cbegin() + curpos, input.cbegin() + end);
            curpos = end;
        } else if (special.find(input.at(curpos)) != std::string::npos) {
            _tokens.emplace(1, input.at(curpos));
            curpos++;
        } else {
            throw std::invalid_argument("Unexpected symbol " + std::string(1, input.at(curpos)));
        }
        curpos = input.find_first_not_of(std::string{' ', '\n', '\r'}, curpos);
    }
}

bool Parser::Tokenizer::empty() const { return _tokens.empty(); }
