//
// Created by Stepan Usatiuk on 22.12.2023.
//

#include <iostream>
#include <set>
#include <utility>

#include "Compiler.h"
#include "Parser.h"
#include "VM.h"


VM::VM(std::istream &instream, std::ostream &outstream) : _instream(instream), _outstream(outstream) {}

void VM::run() {
    while (!_stop) step();
}

void VM::step() {
    Handle poppedH = _c.pop();
    if (poppedH == NIL) {
        _s.push(nullptr);
    } else if (poppedH == LDC) {
        _s.push(_c.pop());
    } else if (poppedH == ATOM) {
        _s.push(_c.pop().atom() ? 1 : 0);
    } else if (poppedH == LD) {
        Handle poppedH2 = _c.pop();

        int64_t frame = poppedH2.car().val();
        int64_t arg = poppedH2.cdr().val();

        assert(frame > 0);
        assert(arg > 0);

        Handle curFrame = _e;

        for (int i = 1; i < frame; i++) {
            curFrame = curFrame.cdr();
        }

        Handle curArg = curFrame.car();

        for (int i = 1; i < arg; i++) {
            curArg = curArg.cdr();
        }

        _s.push(curArg.car());
    } else if (poppedH == SEL) {
        Handle popped2H = _s.pop();

        Handle ct = _c.pop();
        Handle cf = _c.pop();

        _d.push(_c);
        if (popped2H.val() > 0) {
            _c = ct;
        } else {
            _c = cf;
        }
    } else if (poppedH == JOIN) {
        _c = _d.pop();
    } else if (poppedH == LDF) {
        _s.push(Handle::cons(_c.pop(), _e));
    } else if (poppedH == AP) {
        Handle closureH = _s.pop();
        Handle argsH = _s.pop();

        _d.push(_s);
        _d.push(_e);
        _d.push(_c);

        _s = Handle::cons(nullptr, nullptr);
        _c = closureH.car();
        _e = closureH.cdr();
        _e.push(argsH);
    } else if (poppedH == RET) {
        Handle c = _d.pop();
        Handle e = _d.pop();
        Handle s = _d.pop();

        Handle ret = _s.pop();

        _c = c;
        _e = e;
        _s = s;

        _s.push(ret);
    } else if (poppedH == DUM) {
        _e.push(nullptr);
    } else if (poppedH == RAP) {
        Handle closureH = _s.pop();
        Handle argsH = _s.pop();

        Handle origE = _e.cdr();

        _d.push(_s);
        _d.push(origE);
        _d.push(_c);

        _s = Handle::cons(nullptr, nullptr);
        _c = closureH.car();
        _e = closureH.cdr();

        Handle fnEnv = closureH.cdr();
        assert(_e == fnEnv);

        _e.push(argsH);
        fnEnv.setcar(argsH);
    } else if (poppedH == STOP) {
        _stop = true;
    } else if (poppedH == ADD) {
        _s.push(_s.pop().val() + _s.pop().val());
    } else if (poppedH == SUB) {
        assert(false);
    } else if (poppedH == CONS) {
        Handle h1 = _s.pop();
        Handle h2 = _s.pop();

        _s.push(Handle::cons(h1, h2));
    } else if (poppedH == READCHAR) {
        char c;
        _instream >> c;
        _s.push(Handle::makeNumCell(c));
    } else if (poppedH == PUTCHAR) {
        _outstream << (char) _s.pop().val();
    } else if (poppedH == PUTNUM) {
        _outstream << _s.pop().val();
    } else if (poppedH == EVAL) {
        Handle code = _s.pop();
        Handle newc = Compiler::compile(code, nullptr);
        std::cerr << "compiled:\n";
        std::cerr << "(" << code << ")\n";
        std::cerr << "to:\n";
        std::cerr << "(" << newc << ")\n";
        newc.splice(_c);
        _c = newc;
    } else if (poppedH == PRINT) {
        _outstream << _s.pop();
    } else if (poppedH == READ) {
        std::string read;
        std::getline(_instream, read);
        Parser parser;
        parser.loadStr(read);
        _s.push(parser.parseExpr());
    } else {
        assert(false);
    }
}
