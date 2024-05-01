//
// Created by Stepan Usatiuk on 22.12.2023.
//

#ifndef PSIL_CELL_H
#define PSIL_CELL_H

#include <atomic>
#include <cassert>
#include <cstdint>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>

enum class CellType { NUMATOM,
                      STRATOM,
                      CONS };

using CellValType = int64_t;

struct Cell {
    explicit Cell(CellType type) : _type(type) {}
    virtual ~Cell() = 0;

    CellType          _type;
    std::atomic<bool> _live = false;

    virtual void print(std::ostream &out) const = 0;
};

struct NumAtomCell : public Cell {
    NumAtomCell() = delete;
    explicit NumAtomCell(CellValType val) : Cell(CellType::NUMATOM), _val(val) {}

    CellValType _val;

    void print(std::ostream &out) const override { out << _val; }
};

struct StrAtomCell : public Cell {
    StrAtomCell() = delete;
    explicit StrAtomCell(std::string val) : Cell(CellType::STRATOM), _val(std::move(val)) {}

    std::string _val;

    void print(std::ostream &out) const override { out << _val; }
};

struct ConsCell : public Cell {
    ConsCell() : Cell(CellType::CONS) {}
    explicit ConsCell(Cell *car) : Cell(CellType::CONS), _car(car) {}
    ConsCell(Cell *car, Cell *cdr) : Cell(CellType::CONS), _car(car), _cdr(cdr) {}

    std::atomic<Cell *> _car = nullptr;
    std::atomic<Cell *> _cdr = nullptr;

    void print_cons(std::ostream &out, std::set<const Cell *> &seen) const {
        std::stringstream res;
        if (_car) {
            if (_car.load()->_type == CellType::CONS) {
                if (seen.find(_car) == seen.end()) {
                    res << "(";
                    seen.emplace(_car.load());
                    dynamic_cast<ConsCell &>(*_car.load()).print_cons(res, seen);
                    res << ")";
                } else {
                    res << "(recursive)";
                }
            } else {
                _car.load()->print(res);
            }
        } else {
            res << "()";
        }
        if (_cdr) {
            if (_cdr.load()->_type == CellType::CONS) {
                if (seen.find(_cdr) == seen.end()) {
                    res << " ";
                    seen.emplace(_cdr.load());
                    dynamic_cast<ConsCell &>(*_cdr.load()).print_cons(res, seen);
                } else {
                    res << " recursive";
                }
            } else {
                res << ".";
                _cdr.load()->print(res);
            }
        }
        out << res.str();
    }

    void print(std::ostream &out) const override {
        std::stringstream      res;
        std::set<const Cell *> seen{this};
        if (_car) {
            if (_car.load()->_type == CellType::CONS) {
                res << "(";
                seen.emplace(_car.load());
                dynamic_cast<ConsCell &>(*_car.load()).print_cons(res, seen);
                res << ")";
            } else {
                _car.load()->print(res);
            }
        } else {
            res << "()";
        }
        if (_cdr) {
            if (_cdr.load()->_type == CellType::CONS) {
                res << " ";
                seen.emplace(_cdr.load());
                dynamic_cast<ConsCell &>(*_cdr.load()).print_cons(res, seen);
            } else {
                res << ".";
                _cdr.load()->print(res);
            }
        }
        out << res.str();
    }
};

#endif //PSIL_CELL_H
