//
// Created by Stepan Usatiuk on 25.12.2023.
//

#ifndef PSIL_CONSUTILS_H
#define PSIL_CONSUTILS_H

#include "Cell.h"
#include "MemoryContext.h"

namespace ConsUtils {
    static inline MCHandle car(const MCHandle &cell) { return dynamic_cast<ConsCell &>(*cell)._car.load(); }
    static inline MCHandle cdr(const MCHandle &cell) { return dynamic_cast<ConsCell &>(*cell)._cdr.load(); }
    static inline CellValType val(const MCHandle &cell) { return dynamic_cast<NumAtomCell &>(*cell)._val; }
    static inline std::string_view strval(const MCHandle &cell) { return dynamic_cast<StrAtomCell &>(*cell)._val; }
    MCHandle cons(const MCHandle &car, const MCHandle &cdr);
    MCHandle pop(MCHandle &from);
    void push(MCHandle &to, const MCHandle &what);
    void append(MCHandle to, const MCHandle &what);
    MCHandle makeNumCell(int64_t val);
    MCHandle makeStrCell(std::string val);
    void setcar(const MCHandle &to, const MCHandle &car);
    void setcdr(const MCHandle &to, const MCHandle &cdr);
};// namespace ConsUtils


#endif//PSIL_CONSUTILS_H
