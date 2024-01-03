//
// Created by stepus53 on 3.1.24.
//

#ifndef PSIL_COMMAND_H
#define PSIL_COMMAND_H

#include <unordered_map>

#include "Cell.h"
#include "Handle.h"
#include "Options.h"

namespace Command {
    enum CommandE : CellValType {
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
        CAR = 23,
        CDR = 24,
        EQ = 25,
        LT = 26,
        GT = 27,
        NILC = 28,
        MULT = 29,
        DIV = 30
    };

    static inline std::unordered_map<std::string_view, CellValType> str_to_cmd{
            {"NIL", 1},     {"LDC", 2},   {"LD", 3},     {"SEL", 4},   {"JOIN", 5},  {"LDF", 6},  {"AP", 7},        {"RET", 8},
            {"DUM", 9},     {"RAP", 10},  {"STOP", 11},  {"ATOM", 12}, {"ADD", 13},  {"SUB", 14}, {"READCHAR", 15}, {"PUTCHAR", 16},
            {"PUTNUM", 17}, {"EVAL", 18}, {"PRINT", 19}, {"READ", 20}, {"CONS", 21}, {"LDG", 22}, {"CAR", 23},      {"CDR", 24},
            {"EQ", 25},     {"LT", 26},   {"GT", 27},    {"NILC", 28}, {"MULT", 29}, {"DIV", 30}};
    static inline std::unordered_map<CellValType, std::string> cmd_to_str{
            {1, "NIL"},     {2, "LDC"},   {3, "LD"},     {4, "SEL"},   {5, "JOIN"},  {6, "LDF"},  {7, "AP"},        {8, "RET"},
            {9, "DUM"},     {10, "RAP"},  {11, "STOP"},  {12, "ATOM"}, {13, "ADD"},  {14, "SUB"}, {15, "READCHAR"}, {16, "PUTCHAR"},
            {17, "PUTNUM"}, {18, "EVAL"}, {19, "PRINT"}, {20, "READ"}, {21, "CONS"}, {22, "LDG"}, {23, "CAR"},      {24, "CDR"},
            {25, "EQ"},     {26, "LT"},   {27, "GT"},    {28, "NILC"}, {29, "MULT"}, {30, "DIV"}};

    static inline Handle make_cmd(CellValType cmd) {
        if (Options::get_bool("command_strs")) {
            return Handle(cmd_to_str.at(cmd));
        } else
            return Handle(cmd);
    };
};// namespace Command


#endif//PSIL_COMMAND_H
