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
    void loadStr(std::string_view input);
    Handle parseExpr();

private:
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
