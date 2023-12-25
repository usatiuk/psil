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
#include "MemoryContext.h"

class VM {
public:
    VM(std::istream &instream = std::cin, std::ostream &outstream = std::cout);

    void run();

    void loadControl(const MCHandle &h) { _c = h; }

    void step();

private:
    MCHandle _s = nullptr;
    MCHandle _e = nullptr;
    MCHandle _c = nullptr;
    MCHandle _d = nullptr;
    bool _stop = false;

    std::istream &_instream;
    std::ostream &_outstream;

    //    void gc();
};

#endif//PSIL_VM_H
