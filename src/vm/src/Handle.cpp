//
// Created by Stepan Usatiuk on 26.12.2023.
//

#include "Handle.h"

#include "MemoryContext.h"

Handle::Handle(Cell *target) : _target(target) {
    if (target != nullptr)
        MemoryContext::get().add_root(target);
}

Handle::~Handle() {
    if (_target != nullptr)
        MemoryContext::get().remove_root(_target);
}

Handle::Handle(Handle const &other) : _target(other._target) {
    if (_target != nullptr)
        MemoryContext::get().add_root(_target);
}

Handle &Handle::operator=(Handle other) {
    std::swap(_target, other._target);
    return *this;
}

Handle Handle::cons(const Handle &car, const Handle &cdr) {
    auto ret = MemoryContext::get().create_cell<ConsCell>(car.get(), cdr.get());
    return ret;
}

Handle Handle::pop() {
    auto ret = car();
    *this = cdr();
    return ret;
}

void Handle::push(const Handle &what) {
    *this = cons(what, *this);
}

void Handle::append(const Handle &what) {
    if (_target == nullptr) *this = cons(nullptr, nullptr);
    Handle cur = *this;
    if (cur.car().get() == nullptr) {
        cur.setcar(what);
        return;
    }
    while (cur.cdr().get() != nullptr) cur = cur.cdr();
    cur.setcdr(cons(what, nullptr));
}

void Handle::splice(const Handle &what) {
    if (_target == nullptr) {
        *this = what;
        return;
    }
    Handle cur = *this;
    if (cur.car().get() == nullptr) {
        *this = what;
        return;
    }
    while (cur.cdr().get() != nullptr)
        cur = cur.cdr();
    if (what.atom()) cur.setcdr(Handle::cons(what, nullptr));
    else
        cur.setcdr(what);
}

Handle Handle::makeNumCell(int64_t val) {
    return MemoryContext::get().create_cell<NumAtomCell>(val);
}

Handle Handle::makeStrCell(std::string val) {
    return MemoryContext::get().create_cell<StrAtomCell>(std::move(val));
}

void Handle::setcar(const Handle &car) {
    MemoryContext::get().run_dirty<void>([&](std::function<void(Cell *)> dirty) -> void {
        dirty(dynamic_cast<ConsCell &>(*_target)._car);
        dynamic_cast<ConsCell &>(*_target)._car = car.get();
    });
}

void Handle::setcdr(const Handle &cdr) {
    MemoryContext::get().run_dirty<void>([&](std::function<void(Cell *)> dirty) -> void {
        dirty(dynamic_cast<ConsCell &>(*_target)._cdr);
        dynamic_cast<ConsCell &>(*_target)._cdr = cdr.get();
    });
}
Handle::Handle(int64_t val) {
    *this = Handle::makeNumCell(val);
}
Handle::Handle(std::string strval) {
    *this = Handle::makeStrCell(std::move(strval));
}
