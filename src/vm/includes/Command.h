#ifndef PSIL_COMMAND_H
#define PSIL_COMMAND_H

#include <cassert>

namespace Command {
    enum class CommandNum {
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

        ADD = 100,
        SUB = 101,

        READCHAR = 201,
        PUTCHAR = 202,
        PUTNUM = 203,

        CONS = 301,
        END = 1000
    };

    static inline CommandNum int_to_cmd(int64_t _val) {
        assert((_val > 0 && static_cast<CommandNum>(_val) <= CommandNum::END));
        return static_cast<CommandNum>(_val);
    }

    static inline int64_t cmd_to_int(CommandNum _val) {
        return static_cast<int64_t>(_val);
    }
};// namespace Command

#endif