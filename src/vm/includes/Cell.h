//
// Created by Stepan Usatiuk on 22.12.2023.
//

#ifndef PSIL_CELL_H
#define PSIL_CELL_H

#include <cassert>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>

using CellValType = int64_t;

struct Cell {
    virtual ~Cell() = 0;

    std::atomic<bool> _live = false;

    virtual void print(std::ostream &out) = 0;
};

struct NumAtomCell : public Cell {
    NumAtomCell() = delete;
    explicit NumAtomCell(CellValType val) : _val(val) {}

    CellValType _val;

    void print(std::ostream &out) override {
        out << _val;
    }
};

struct StrAtomCell : public Cell {
    StrAtomCell() = delete;
    explicit StrAtomCell(std::string val) : _val(std::move(val)) {}

    std::string _val;

    void print(std::ostream &out) override {
        out << _val;
    }
};

struct ConsCell : public Cell {
    explicit ConsCell(Cell *car) : _car(car) {}
    ConsCell(Cell *car, Cell *cdr) : _car(car), _cdr(cdr) {}

    std::atomic<Cell *> _car = nullptr;
    std::atomic<Cell *> _cdr = nullptr;

    void print(std::ostream &out) override {
        std::stringstream res;
        if (_car) {
            if (dynamic_cast<ConsCell *>(_car.load())) {
                res << "(";
                _car.load()->print(res);
                res << ")";
            } else {
                _car.load()->print(res);
            }
        }
        if (_cdr) {
            if (dynamic_cast<ConsCell *>(_cdr.load())) {
                res << " ";
                _cdr.load()->print(res);
            } else {
                res << ".";
                _cdr.load()->print(res);
            }
        }
        out << res.str();
    }
};

#endif//PSIL_CELL_H
