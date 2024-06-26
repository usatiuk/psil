//
// Created by Stepan Usatiuk on 27.12.2023.
//

#ifndef PSIL_COMPILER_H
#define PSIL_COMPILER_H


#include "Handle.h"

class Compiler {
public:
    static Handle compile(const Handle &src, Handle fake_env = nullptr, const Handle &suffix = nullptr);

private:
    static Handle findIndex(const Handle &symbol, const Handle &env);
};


#endif //PSIL_COMPILER_H
