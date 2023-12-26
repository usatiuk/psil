//
// Created by Stepan Usatiuk on 22.12.2023.
//

#ifndef PSIL_CELL_H
#define PSIL_CELL_H

#include <cassert>
#include <cstdint>
#include <string>
#include <utility>

enum class CellType {
    NUMATOM,
    STRATOM,
    CONS
};

using CellValType = int64_t;

struct Cell {
    explicit Cell(CellType type) : _type(type) {}
    virtual ~Cell() = 0;

    CellType _type;
    bool live = false;
};

struct NumAtomCell : public Cell {
    NumAtomCell() = delete;
    explicit NumAtomCell(CellValType val) : Cell(CellType::NUMATOM), _val(val) {}

    CellValType _val;
};

struct StrAtomCell : public Cell {
    StrAtomCell() = delete;
    explicit StrAtomCell(std::string val) : Cell(CellType::STRATOM), _val(std::move(val)) {}

    std::string _val;
};

struct ConsCell : public Cell {
    ConsCell() : Cell(CellType::CONS) {}
    explicit ConsCell(Cell *car) : Cell(CellType::CONS), _car(car) {}
    ConsCell(Cell *car, Cell *cdr) : Cell(CellType::CONS), _car(car), _cdr(cdr) {}

    Cell *_car = nullptr;
    Cell *_cdr = nullptr;
};

#endif//PSIL_CELL_H
