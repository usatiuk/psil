//
// Created by Stepan Usatiuk on 26.12.2023.
//

#include "Handle.h"

#include "MemoryContext.h"

Handle::Handle(Cell *target) : _target(target) {
    if (target != nullptr)
        CURRENT_MC.load()->add_root(target);
}

Handle::~Handle() {
    if (_target != nullptr)
        CURRENT_MC.load()->remove_root(_target);
}

Handle::Handle(Handle const &other) : _target(other._target) {
    if (_target != nullptr)
        CURRENT_MC.load()->add_root(_target);
}

Handle &Handle::operator=(Handle other) {
    std::swap(_target, other._target);
    return *this;
}

Handle Handle::cons(const Handle &car, const Handle &cdr) {
    auto ret = CURRENT_MC.load()->create_cell<ConsCell>(car.get(), cdr.get());
    return ret;
}

Handle Handle::pop(Handle &from) {
    auto ret = from.car();
    from = from.cdr();
    return ret;
}

void Handle::push(Handle &to, const Handle &what) {
    to = cons(what, to);
}

void Handle::append(Handle to, const Handle &what) {
    assert(to.get() != nullptr);
    if (to.car().get() == nullptr) {
        to.setcar(what);
        return;
    }
    while (to.cdr().get() != nullptr) to = to.cdr();
    to.setcdr(cons(what, nullptr));
}

Handle Handle::makeNumCell(int64_t val) {
    return CURRENT_MC.load()->create_cell<NumAtomCell>(val);
}

Handle Handle::makeStrCell(std::string val) {
    return CURRENT_MC.load()->create_cell<StrAtomCell>(std::move(val));
}

void Handle::setcar(const Handle &car) {
    CURRENT_MC.load()->run_dirty<void>([&](std::function<void(Cell *)> dirty) -> void {
        dirty(dynamic_cast<ConsCell &>(*_target)._car);
        dynamic_cast<ConsCell &>(*_target)._car = car.get();
    });
}

void Handle::setcdr(const Handle &cdr) {
    CURRENT_MC.load()->run_dirty<void>([&](std::function<void(Cell *)> dirty) -> void {
        dirty(dynamic_cast<ConsCell &>(*_target)._cdr);
        dynamic_cast<ConsCell &>(*_target)._cdr = cdr.get();
    });
}
