//
// Created by Stepan Usatiuk on 22.12.2023.
//

#include <iostream>
#include <set>
#include <utility>

#include "VM.h"


VM::VM(std::istream &instream, std::ostream &outstream) : _instream(instream), _outstream(outstream) {}

void VM::run() {
    while (!_stop) step();
}

void VM::step() {
    Handle poppedH = Handle::pop(_c);
    if (poppedH == NIL) {
        Handle::push(_s, nullptr);
    } else if (poppedH == LDC) {
        Handle::push(_s, Handle::pop(_c));
    } else if (poppedH == LD) {
        Handle poppedH2 = Handle::pop(_c);

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

        Handle::push(_s, curArg.car());
    } else if (poppedH == SEL) {

        Handle popped2H = Handle::pop(_s);

        Handle ct = Handle::pop(_c);
        Handle cf = Handle::pop(_c);

        Handle::push(_d, _c);
        if (popped2H.val() > 0) {
            _c = ct;
        } else {
            _c = cf;
        }
    } else if (poppedH == JOIN) {
        _c = Handle::pop(_d);
    } else if (poppedH == LDF) {
        Handle::push(_s, Handle::cons(Handle::pop(_c), _e));
    } else if (poppedH == AP) {
        Handle closureH = Handle::pop(_s);
        Handle argsH = Handle::pop(_s);

        Handle::push(_d, _s);
        Handle::push(_d, _e);
        Handle::push(_d, _c);

        _s = Handle::cons(nullptr, nullptr);
        _c = closureH.car();
        _e = closureH.cdr();
        Handle::push(_e, argsH);
    } else if (poppedH == RET) {
        Handle c = Handle::pop(_d);
        Handle e = Handle::pop(_d);
        Handle s = Handle::pop(_d);

        Handle ret = Handle::pop(_s);

        _c = c;
        _e = e;
        _s = s;

        Handle::push(_s, ret);
    } else if (poppedH == DUM) {
        Handle::push(_e, nullptr);
    } else if (poppedH == RAP) {
        Handle closureH = Handle::pop(_s);
        Handle argsH = Handle::pop(_s);


        Handle origE = _e.cdr();

        Handle::push(_d, _s);
        Handle::push(_d, origE);
        Handle::push(_d, _c);

        _s = Handle::cons(nullptr, nullptr);
        _c = closureH.car();
        _e = closureH.cdr();

        Handle fnEnv = closureH.cdr();
        //        assert(_e.get() == fnEnv.get());

        Handle::push(_e, argsH);
        fnEnv.setcar(argsH);
    } else if (poppedH == STOP) {
        _stop = true;
    } else if (poppedH == ADD) {
        int64_t ret = Handle::pop(_s).val() + Handle::pop(_s).val();
        Handle::push(_s, Handle::makeNumCell(ret));
    } else if (poppedH == SUB) {
        assert(false);
        int64_t ret = Handle::pop(_s).val() + Handle::pop(_s).val();
        Handle::push(_s, Handle::makeNumCell(ret));
    } else if (poppedH == CONS) {
        Handle h1 = Handle::pop(_s);
        Handle h2 = Handle::pop(_s);

        Handle::push(_s, Handle::cons(h1, h2));
    } else if (poppedH == READCHAR) {
        char c;
        _instream >> c;
        Handle::push(_s, Handle::makeNumCell(c));
    } else if (poppedH == PUTCHAR) {
        _outstream << (char) Handle::pop(_s).val();
    } else if (poppedH == PUTNUM) {
        _outstream << Handle::pop(_s).val();
    } else {
        assert(false);
    }
}
