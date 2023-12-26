//
// Created by Stepan Usatiuk on 22.12.2023.
//

#ifndef PSIL_VM_H
#define PSIL_VM_H

#include <iostream>
#include <list>
#include <utility>
#include <vector>

#include "Cell.h"
#include "ConsUtils.h"
#include "MemoryContext.h"

class VM {
public:
    VM(std::istream &instream = std::cin, std::ostream &outstream = std::cout);

    void run();

    void loadControl(const MCHandle &h) { _c = h; }

    void step();

private:
    MCHandle _s = ConsUtils::cons(nullptr, nullptr);
    MCHandle _e = ConsUtils::cons(nullptr, nullptr);
    MCHandle _c = ConsUtils::cons(nullptr, nullptr);
    MCHandle _d = ConsUtils::cons(nullptr, nullptr);
    bool _stop = false;

    std::istream &_instream;
    std::ostream &_outstream;

    //    void gc();
};

#endif//PSIL_VM_H
