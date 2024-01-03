//
// Created by stepus53 on 3.1.24.
//

#ifndef PSIL_COMMAND_H
#define PSIL_COMMAND_H

#include <unordered_map>

#include "Cell.h"

namespace Command {
    enum Command : CellValType {
        NIL = 1,
        LDC = 2,
        LD = 3,
        SEL = 4,
        JOIN = 5,
        LDF = 6,
        AP = 7,
        RET = 8,
        DUM = 9,
        RAP = 10,
        STOP = 11,

        ATOM = 12,
        ADD = 13,
        SUB = 14,

        READCHAR = 15,
        PUTCHAR = 16,
        PUTNUM = 17,

        EVAL = 18,
        PRINT = 19,
        READ = 20,

        CONS = 21,
        LDG = 22,
    };

    static inline std::unordered_map<std::string_view, CellValType> str_to_cmd{
            {"NIL", 1},       {"LDC", 2},      {"LD", 3},      {"SEL", 4},   {"JOIN", 5},   {"LDF", 6},   {"AP", 7},
            {"RET", 8},       {"DUM", 9},      {"RAP", 10},    {"STOP", 11}, {"ATOM", 12},  {"ADD", 13},  {"SUB", 14},
            {"READCHAR", 15}, {"PUTCHAR", 16}, {"PUTNUM", 17}, {"EVAL", 18}, {"PRINT", 19}, {"READ", 20}, {"CONS", 21},
    };
};// namespace Command


#endif//PSIL_COMMAND_H
