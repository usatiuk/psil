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

    MCHandle NIL = ConsUtils::makeStrCell("NIL");
    MCHandle LDC = ConsUtils::makeStrCell("LDC");
    MCHandle LD = ConsUtils::makeStrCell("LD");
    MCHandle SEL = ConsUtils::makeStrCell("SEL");
    MCHandle JOIN = ConsUtils::makeStrCell("JOIN");
    MCHandle LDF = ConsUtils::makeStrCell("LDF");
    MCHandle AP = ConsUtils::makeStrCell("AP");
    MCHandle RET = ConsUtils::makeStrCell("RET");
    MCHandle DUM = ConsUtils::makeStrCell("DUM");
    MCHandle RAP = ConsUtils::makeStrCell("RAP");
    MCHandle STOP = ConsUtils::makeStrCell("STOP");

    MCHandle ADD = ConsUtils::makeStrCell("ADD");
    MCHandle SUB = ConsUtils::makeStrCell("SUB");

    MCHandle READCHAR = ConsUtils::makeStrCell("READCHAR");
    MCHandle PUTCHAR = ConsUtils::makeStrCell("PUTCHAR");
    MCHandle PUTNUM = ConsUtils::makeStrCell("PUTNUM");

    MCHandle CONS = ConsUtils::makeStrCell("CONS");

    //    void gc();
};

#endif//PSIL_VM_H
