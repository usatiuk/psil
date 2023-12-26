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

void ConsUtils::push(MCHandle &to, const MCHandle &what) {
    to = cons(what, to);
}

void ConsUtils::append(MCHandle to, const MCHandle &what) {
    assert(to.get() != nullptr);
    if (car(to).get() == nullptr) {
        setcar(to, what);
        return;
    }
    while (cdr(to).get() != nullptr) to = cdr(to);
    setcdr(to, cons(what, nullptr));
}

MCHandle ConsUtils::makeNumCell(int64_t val) {
    return CURRENT_MC.load()->create_cell<NumAtomCell>(val);
}

MCHandle ConsUtils::makeStrCell(std::string val) {
    return CURRENT_MC.load()->create_cell<StrAtomCell>(std::move(val));
}

void ConsUtils::setcar(const MCHandle &to, const MCHandle &car) {
    CURRENT_MC.load()->run_dirty(to, [&] {
        dynamic_cast<ConsCell &>(*to)._car = car.get();
    });
}

void ConsUtils::setcdr(const MCHandle &to, const MCHandle &cdr) {
    CURRENT_MC.load()->run_dirty(to, [&] {
        dynamic_cast<ConsCell &>(*to)._cdr = cdr.get();
    });
}
