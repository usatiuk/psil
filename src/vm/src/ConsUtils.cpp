//
// Created by Stepan Usatiuk on 25.12.2023.
//

#include "ConsUtils.h"

#include "MemoryContext.h"

MCHandle ConsUtils::cons(const MCHandle &car, const MCHandle &cdr) {
    auto ret = CURRENT_MC.load()->create_cell<ConsCell>(car.get(), cdr.get());
    return ret;
}

MCHandle ConsUtils::pop(MCHandle &from) {
    auto ret = car(from);
    from = cdr(from);
    return ret;
}

MCHandle ConsUtils::push(MCHandle &to, const MCHandle &what) {
    to = cons(what, to);
    return to;
}

void ConsUtils::append(MCHandle to, const MCHandle &what) {
    assert(to.get() != nullptr);
    if (car(to).get() == nullptr) {
        dynamic_cast<ConsCell &>(*to)._car = what.get();
        return;
    }
    while (dynamic_cast<ConsCell &>(*to)._cdr != nullptr) to = cdr(to);
    dynamic_cast<ConsCell &>(*to)._cdr = cons(what, nullptr).get();
}

MCHandle ConsUtils::makeIntCell(int64_t val) {
    return CURRENT_MC.load()->create_cell<ValueCell>(val);
}

void ConsUtils::setcar(const MCHandle &to, const MCHandle &car) {
    dynamic_cast<ConsCell &>(*to)._car = car.get();
}
