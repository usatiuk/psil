//
// Created by Stepan Usatiuk on 22.12.2023.
//

#include <iostream>
#include <set>
#include <utility>

#include "Command.h"
#include "Compiler.h"
#include "Parser.h"
#include "VM.h"

using namespace Command;

VM::VM(std::istream &instream, std::ostream &outstream) : _instream(instream), _outstream(outstream) {}

void VM::run() {
    while (!_stop) step();
}

void VM::step() {
    Handle poppedH = _c.pop();
    // as to not complicate parser for tests...
    CellValType poppedCmd = poppedH.type() == CellType::STRATOM ? str_to_cmd.at(poppedH.strval()) : poppedH.val();
    if (poppedCmd == NIL) {
        _s.push(nullptr);
    } else if (poppedCmd == LDC) {
        _s.push(_c.pop());
    } else if (poppedCmd == ATOM) {
        _s.push(_c.pop().atom() ? 1 : 0);
    } else if (poppedCmd == LD) {
        Handle poppedH2 = _c.pop();

        int64_t frame = poppedH2.car().val();
        int64_t arg = poppedH2.cdr().val();

        assert(frame > 0);
        assert(arg > 0);

        Handle curFrame = _e;

        for (int i = 1; i < frame; i++) { curFrame = curFrame.cdr(); }

        Handle curArg = curFrame.car();

        for (int i = 1; i < arg; i++) { curArg = curArg.cdr(); }

        _s.push(curArg.car());
    } else if (poppedCmd == SEL) {
        Handle popped2H = _s.pop();

        Handle ct = _c.pop();
        Handle cf = _c.pop();

        _d.push(_c);
        if (popped2H.val() > 0) {
            _c = ct;
        } else {
            _c = cf;
        }
    } else if (poppedCmd == JOIN) {
        _c = _d.pop();
    } else if (poppedCmd == LDF) {
        _s.push(Handle::cons(_c.pop(), _e));
    } else if (poppedCmd == AP) {
        Handle closureH = _s.pop();
        Handle argsH = _s.pop();

        _d.push(_s);
        _d.push(_e);
        _d.push(_c);

        _s = Handle::cons(nullptr, nullptr);
        _c = closureH.car();
        _e = closureH.cdr();
        _e.push(argsH);
    } else if (poppedCmd == RET) {
        Handle c = _d.pop();
        Handle e = _d.pop();
        Handle s = _d.pop();

        Handle ret = _s.pop();

        _c = c;
        _e = e;
        _s = s;

        _s.push(ret);
    } else if (poppedCmd == DUM) {
        _e.push(nullptr);
    } else if (poppedCmd == RAP) {
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
    } else if (poppedCmd == STOP) {
        _stop = true;
    } else if (poppedCmd == ADD) {
        _s.push(_s.pop().val() + _s.pop().val());
    } else if (poppedCmd == SUB) {
        assert(false);
    } else if (poppedCmd == CONS) {
        Handle h1 = _s.pop();
        Handle h2 = _s.pop();

        _s.push(Handle::cons(h1, h2));
    } else if (poppedCmd == READCHAR) {
        char c;
        _instream >> c;
        _s.push(Handle::makeNumCell(c));
    } else if (poppedCmd == PUTCHAR) {
        _outstream << (char) _s.pop().val();
    } else if (poppedCmd == PUTNUM) {
        _outstream << _s.pop().val();
    } else if (poppedCmd == EVAL) {
        Handle code = _s.pop();
        Handle newc = Compiler::compile(code, _globals_names);
        Logger::log(
                "Compiler",
                [&](std::ostream &out) {
                    out << "Compiled (" << code << ")\n";
                    out << "to\n";
                    out << "(" << newc << ")";
                },
                Logger::DEBUG);
        newc.splice(_c);
        _c = newc;
    } else if (poppedCmd == LDG) {
        _globals_vals.append(Handle::cons(_c.pop(), _e));
    } else if (poppedCmd == PRINT) {
        _outstream << _s.pop();
    } else if (poppedCmd == READ) {
        std::string read;
        std::getline(_instream, read);
        Parser parser;
        parser.loadStr(read);
        _s.push(parser.parseExpr());
    } else {
        assert(false);
    }
}
