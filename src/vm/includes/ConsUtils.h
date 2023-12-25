//
// Created by Stepan Usatiuk on 25.12.2023.
//

#ifndef PSIL_CONSUTILS_H
#define PSIL_CONSUTILS_H

#include "Cell.h"
#include "MemoryContext.h"

namespace ConsUtils {
    static inline MCHandle car(const MCHandle &cell) { return dynamic_cast<ConsCell &>(*cell)._car; }
    static inline MCHandle cdr(const MCHandle &cell) { return dynamic_cast<ConsCell &>(*cell)._cdr; }
    MCHandle cons(const MCHandle &car, const MCHandle &cdr);
    MCHandle pop(MCHandle &from);
    MCHandle push(MCHandle &to, const MCHandle &what);
    void append(MCHandle to, const MCHandle &what);
    MCHandle makeIntCell(int64_t val);
};// namespace ConsUtils


#endif//PSIL_CONSUTILS_H
