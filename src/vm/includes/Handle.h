//
// Created by Stepan Usatiuk on 26.12.2023.
//

#ifndef PSIL_HANDLE_H
#define PSIL_HANDLE_H

#include "Cell.h"

class MemoryContext;

class Handle {
public:
    friend MemoryContext;

    Handle(Cell *target);
    ~Handle();

    Handle(Handle const &other);
    Handle &operator=(Handle other);

    bool operator==(const Handle &rhs) const {
        return _target == rhs._target;
    }

    Handle car() { return dynamic_cast<ConsCell &>(*_target)._car.load(); }
    Handle cdr() { return dynamic_cast<ConsCell &>(*_target)._cdr.load(); }
    CellValType val() { return dynamic_cast<NumAtomCell &>(*_target)._val; }
    std::string_view strval() { return dynamic_cast<StrAtomCell &>(*_target)._val; }


    static Handle cons(const Handle &car, const Handle &cdr);
    static Handle pop(Handle &from);
    static void push(Handle &to, const Handle &what);
    static void append(Handle to, const Handle &what);
    static Handle makeNumCell(int64_t val);
    static Handle makeStrCell(std::string val);
    void setcar(const Handle &car);
    void setcdr(const Handle &cdr);

private:
    Cell *operator->() const { return _target; }
    Cell &operator*() const { return *_target; }
    Cell *get() const noexcept { return _target; }

    Cell *_target = nullptr;
};

#endif//PSIL_HANDLE_H
