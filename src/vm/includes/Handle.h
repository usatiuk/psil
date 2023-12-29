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

    Handle(Cell *target = nullptr);
    Handle(int64_t val);
    Handle(std::string strval);
    ~Handle();

    Handle(Handle const &other);
    Handle &operator=(Handle other);

    bool operator==(const Handle &rhs) const {
        if (_target == rhs._target) {
            return true;
        } else if ((_target != nullptr && rhs._target != nullptr) && (typeid(_target) == typeid(rhs._target))) {
            if (auto p = dynamic_cast<NumAtomCell *>(_target)) {
                return dynamic_cast<NumAtomCell &>(*_target)._val == dynamic_cast<NumAtomCell &>(*rhs._target)._val;
            } else if (auto p = dynamic_cast<StrAtomCell *>(_target)) {
                return dynamic_cast<StrAtomCell &>(*_target)._val == dynamic_cast<StrAtomCell &>(*rhs._target)._val;
            }
        }
        return false;
    }

    static Handle cons(const Handle &car, const Handle &cdr);

    Handle car() { return dynamic_cast<ConsCell &>(*_target)._car.load(); }
    Handle cdr() { return dynamic_cast<ConsCell &>(*_target)._cdr.load(); }
    CellValType val() { return dynamic_cast<NumAtomCell &>(*_target)._val; }
    std::string_view strval() { return dynamic_cast<StrAtomCell &>(*_target)._val; }

    bool num() const {
        return dynamic_cast<NumAtomCell *>(_target) != nullptr;
    }
    bool str() const {
        return dynamic_cast<StrAtomCell *>(_target) != nullptr;
    }

    bool atom() const {
        if (!_target) return false;
        return dynamic_cast<ConsCell *>(_target) == nullptr;
    }

    bool null() {
        if (!_target) return true;
        if (auto p = dynamic_cast<ConsCell *>(_target)) {
            return car() == nullptr && cdr() == nullptr;
        }
        return false;
    }

    Handle pop();
    void push(const Handle &what);
    void append(const Handle &what);
    void splice(const Handle &what);

    static Handle makeNumCell(int64_t val);
    static Handle makeStrCell(std::string val);
    void setcar(const Handle &car);
    void setcdr(const Handle &cdr);

    friend std::ostream &operator<<(std::ostream &stream, const Handle &h) {
        if (h._target)
            h._target->print(stream);
        return stream;
    }

private:
    Cell *operator->() const { return _target; }
    Cell &operator*() const { return *_target; }
    Cell *get() const noexcept { return _target; }

    Cell *_target = nullptr;
};

#endif//PSIL_HANDLE_H
