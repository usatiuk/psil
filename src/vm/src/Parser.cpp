//
// Created by Stepan Usatiuk on 23.12.2023.
//

#include "Parser.h"

#include <ranges>
#include <stack>

#include "ConsUtils.h"
#include "MemoryContext.h"
#include "VM.h"


void Parser::loadStr(std::string_view input) {
    _tokenizer.load(input);
}

MCHandle Parser::parseExpr() {
    while (!_tokenizer.empty()) {
        std::string token = std::string(_tokenizer.peek());
        if (token == "(") {
            _tokenizer.getNext();
            MCHandle out(ConsUtils::cons(nullptr, nullptr));
            while (token != ")") {
                if (token == ".") {
                    _tokenizer.getNext();

                    ConsUtils::setcdr(out, parseExpr());

                    if (_tokenizer.getNext() != ")")
                        throw std::invalid_argument("Missing ) after pair");

                    return out;
                }
                ConsUtils::append(out, parseExpr());
                token = _tokenizer.peek();
            }
            _tokenizer.getNext();
            return out;
        } else {
            token = _tokenizer.getNext();
            try {
                CellValType val = std::stoi(token);
                return ConsUtils::makeNumCell(val);
            } catch (...) {
                return ConsUtils::makeStrCell(token);
            }
        }
    }

    assert(false);
}
std::string Parser::Tokenizer::getNext() {
    std::string ret = std::move(_tokens.front());
    _tokens.pop();
    return ret;
}

std::string_view Parser::Tokenizer::peek() const {
    return _tokens.front();
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
