//
// Created by Stepan Usatiuk on 22.12.2023.
//

#ifndef PSIL_CELL_H
#define PSIL_CELL_H

#include <cstdint>
#include <cassert>

enum class CellType {
    INT,
    CONS
};

struct Cell {
    explicit Cell(CellType type) : _type(type) {}

    virtual ~Cell() = 0;

    CellType _type;
};

struct IntCell : public Cell {
    IntCell() : Cell(CellType::INT) {}

    IntCell(int64_t val) : Cell(CellType::INT), _val(val) {}

    int64_t _val{};
};

struct CommandCell : public IntCell {
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
        END = 1000
    };

    CommandCell(CommandNum cmd) : IntCell(static_cast<int64_t>(cmd)) {}

    CommandNum intToCmd() {
        assert((_val > 0 && static_cast<CommandNum>(_val) <= CommandNum::END));
        return static_cast<CommandNum>(_val);
    }

};

struct ConsCell : public Cell {
    ConsCell() : Cell(CellType::CONS) {}

    ConsCell(Cell *car) : Cell(CellType::CONS), _car(car) {}

    ConsCell(Cell *car, Cell *cdr) : Cell(CellType::CONS), _car(car), _cdr(cdr) {}

    Cell *_car = nullptr;
    Cell *_cdr = nullptr;
};

#endif //PSIL_CELL_H
