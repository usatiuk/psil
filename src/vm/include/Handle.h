//
// Created by Stepan Usatiuk on 26.12.2023.
//

#ifndef PSIL_HANDLE_H
#define PSIL_HANDLE_H

#include "Cell.h"

class MemoryContext;
class Handle;

template<>
class std::hash<Handle>;

class Handle {
public:
    friend MemoryContext;
    friend class std::hash<Handle>;

    Handle(Cell *target = nullptr);
    Handle(int64_t val);
    Handle(std::string strval);
    ~Handle();

    Handle(Handle const &other);
    Handle &operator=(Handle other);

    bool operator==(const Handle &rhs) const {
        if (_target == rhs._target) {
            return true;
        } else if ((_target != nullptr && rhs._target != nullptr) && (_target->_type == rhs._target->_type)) {
            if (_target->_type == CellType::NUMATOM) {
                return dynamic_cast<NumAtomCell &>(*_target)._val == dynamic_cast<NumAtomCell &>(*rhs._target)._val;
            } else if (_target->_type == CellType::STRATOM) {
                return dynamic_cast<StrAtomCell &>(*_target)._val == dynamic_cast<StrAtomCell &>(*rhs._target)._val;
            }
        } else if (null() && rhs.null())
            return true;
        return false;
    }

    static Handle cons(const Handle &car, const Handle &cdr);

    const Handle car() const {
        if (!_target) return Handle(nullptr);
        return dynamic_cast<ConsCell &>(*_target)._car.load();
    }
    const Handle cdr() const {
        if (!_target) return Handle(nullptr);
        return dynamic_cast<ConsCell &>(*_target)._cdr.load();
    }
    Handle car() {
        if (!_target) return Handle(nullptr);
        return dynamic_cast<ConsCell &>(*_target)._car.load();
    }
    Handle cdr() {
        if (!_target) return Handle(nullptr);
        return dynamic_cast<ConsCell &>(*_target)._cdr.load();
    }

    CellValType val() const { return dynamic_cast<NumAtomCell &>(*_target)._val; }
    std::string_view strval() const { return dynamic_cast<StrAtomCell &>(*_target)._val; }

    CellType type() const {
        if (!_target) return CellType::CONS;
        return _target->_type;
    }

    bool atom() const { return type() != CellType::CONS; }

    bool null() const {
        if (!_target) return true;
        if (type() == CellType::CONS && car() == nullptr && cdr() == nullptr) return true;
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
        if (h._target) h._target->print(stream);
        return stream;
    }

private:
    Cell *operator->() const { return _target; }
    Cell &operator*() const { return *_target; }
    Cell *get() const noexcept { return _target; }

    Cell *_target = nullptr;
};

namespace std {
    template<>
    class hash<Handle> {
    public:
        size_t operator()(const Handle &c) const {
            if (c.type() == CellType::NUMATOM) return std::hash<CellValType>()(c.val());
            else if (c.type() == CellType::STRATOM)
                return std::hash<std::string_view>()(c.strval());
            else
                return std::hash<uintptr_t>()((uintptr_t) c.get());
        }
    };
}// namespace std

#endif//PSIL_HANDLE_H
