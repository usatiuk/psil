//
// Created by Stepan Usatiuk on 22.12.2023.
//

#ifndef PSIL_CELL_H
#define PSIL_CELL_H

#include <cassert>
#include <cstdint>

enum class CellType {
    INT,
    CONS
};

using CellValType = int64_t;

struct Cell {
    explicit Cell(CellType type) : _type(type) {}
    virtual ~Cell() = 0;

    CellType _type;
    bool live = false;
};

struct ValueCell : public Cell {
    ValueCell() = delete;
    explicit ValueCell(CellValType val) : Cell(CellType::INT), _val(val) {}

    CellValType _val;
};

struct ConsCell : public Cell {
    ConsCell() : Cell(CellType::CONS) {}
    explicit ConsCell(Cell *car) : Cell(CellType::CONS), _car(car) {}
    ConsCell(Cell *car, Cell *cdr) : Cell(CellType::CONS), _car(car), _cdr(cdr) {}

    Cell *_car = nullptr;
    Cell *_cdr = nullptr;
};

#endif//PSIL_CELL_H
