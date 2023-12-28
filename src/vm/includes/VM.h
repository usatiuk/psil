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

    void loadControl(const Handle &h) { _c = h; }

    void step();

private:
    Handle _s = Handle::cons(nullptr, nullptr);
    Handle _e = Handle::cons(nullptr, nullptr);
    Handle _c = Handle::cons(nullptr, nullptr);
    Handle _d = Handle::cons(nullptr, nullptr);
    bool _stop = false;

    std::istream &_instream;
    std::ostream &_outstream;

    Handle NIL = Handle::makeStrCell("NIL");
    Handle LDC = Handle::makeStrCell("LDC");
    Handle LD = Handle::makeStrCell("LD");
    Handle SEL = Handle::makeStrCell("SEL");
    Handle JOIN = Handle::makeStrCell("JOIN");
    Handle LDF = Handle::makeStrCell("LDF");
    Handle AP = Handle::makeStrCell("AP");
    Handle RET = Handle::makeStrCell("RET");
    Handle DUM = Handle::makeStrCell("DUM");
    Handle RAP = Handle::makeStrCell("RAP");
    Handle STOP = Handle::makeStrCell("STOP");

    Handle ATOM = Handle::makeStrCell("ATOM");
    Handle ADD = Handle::makeStrCell("ADD");
    Handle SUB = Handle::makeStrCell("SUB");

    Handle READCHAR = Handle::makeStrCell("READCHAR");
    Handle PUTCHAR = Handle::makeStrCell("PUTCHAR");
    Handle PUTNUM = Handle::makeStrCell("PUTNUM");

    Handle EVAL = Handle::makeStrCell("EVAL");
    Handle PRINT = Handle::makeStrCell("PRINT");

    Handle CONS = Handle::makeStrCell("CONS");
};

#endif//PSIL_VM_H
