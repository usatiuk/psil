//
// Created by Stepan Usatiuk on 23.12.2023.
//

#ifndef PSIL_PARSER_H
#define PSIL_PARSER_H

#include <queue>
#include <string>
#include <string_view>


#include "MemoryContext.h"

class VM;

struct Cell;

class Parser {
public:
    explicit Parser(VM &vm);

    void loadSecd(std::string_view input);

private:
    void compileBody(const std::function<void(MCHandle)> &sink);

    VM &_vm;

    class Tokenizer {
    public:
        void load(std::string_view input);

        std::string getNext();

        std::string_view peek() const;

        bool empty() const;

    private:
        std::queue<std::string> _tokens;
    };

    Tokenizer _tokenizer;
};


#endif//PSIL_PARSER_H
