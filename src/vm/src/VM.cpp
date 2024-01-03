//
// Created by Stepan Usatiuk on 22.12.2023.
//

#include <iostream>
#include <optional>
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


    Logger::log(
            "VM",
            [&](std::ostream &out) {
                out << "Executing " << cmd_to_str.at(poppedCmd) << "\n";
                if (!_s.null()) out << "s:" << _s << "\n";
                if (!_e.null()) out << "e:" << _e << "\n";
                if (!_c.null()) out << "c:" << _c << "\n";
                if (!_d.null()) out << "d:" << _d << "\n";
            },
            Logger::TRACE);

    if (poppedCmd == NIL) {
        _s.push(nullptr);
    } else if (poppedCmd == LDC) {
        _s.push(_c.pop());
    } else if (poppedCmd == ATOM) {
        _s.push(_s.pop().atom() ? 1 : 0);
    } else if (poppedCmd == NILC) {
        _s.push(_s.pop().null() ? 1 : 0);
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

        std::optional<std::string> name;
        if (Logger::en_level("VM", Logger::DEBUG)) {
            name = "unknown";
            for (const auto &p: _globals_names_map) {
                if (p.first == closureH) name = p.second;
            }
            _d.push(Handle(*name));
        }

        _s = Handle::cons(nullptr, nullptr);
        _c = closureH.car();
        _e = closureH.cdr();

        Logger::log(
                "VM",
                [&](std::ostream &out) {
                    out << _cur_call_level;
                    for (int i = 0; i < _cur_call_level; i++) out << "  ";
                    out << " Applying " << *name;
                    out << " with args " << argsH;
                },
                Logger::DEBUG);

        _cur_call_level++;
        _e.push(argsH);
    } else if (poppedCmd == RET) {
        _cur_call_level--;

        Handle n;
        if (Logger::en_level("VM", Logger::DEBUG)) n = _d.pop();

        Handle c = _d.pop();
        Handle e = _d.pop();
        Handle s = _d.pop();

        Handle ret = _s.pop();

        _c = c;
        _e = e;
        _s = s;

        Logger::log(
                "VM",
                [&](std::ostream &out) {
                    out << _cur_call_level;
                    for (int i = 0; i < _cur_call_level; i++) out << "  ";
                    out << " Returning from " << n << " ";
                    bool cons = !ret.atom();
                    if (cons) out << "(";
                    out << ret;
                    if (cons) out << ")";
                },
                Logger::DEBUG);

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

        if (Logger::en_level("VM", Logger::DEBUG)) _d.push(Handle("rap"));

        _s = Handle::cons(nullptr, nullptr);
        _c = closureH.car();
        _e = closureH.cdr();

        Handle fnEnv = closureH.cdr();
        assert(_e == fnEnv);

        _cur_call_level++;

        _e.push(argsH);
        fnEnv.setcar(argsH);
    } else if (poppedCmd == STOP) {
        _stop = true;
    } else if (poppedCmd == ADD) {
        _s.push(_s.pop().val() + _s.pop().val());
    } else if (poppedCmd == MULT) {
        _s.push(_s.pop().val() * _s.pop().val());
    } else if (poppedCmd == SUB) {
        CellValType a1 = _s.pop().val();
        CellValType a2 = _s.pop().val();
        _s.push(a1 - a2);
    } else if (poppedCmd == DIV) {
        CellValType a1 = _s.pop().val();
        CellValType a2 = _s.pop().val();
        _s.push(a1 / a2);
    } else if (poppedCmd == EQ) {
        _s.push(_s.pop() == _s.pop() ? 1 : 0);
    } else if (poppedCmd == LT) {
        CellValType a1 = _s.pop().val();
        CellValType a2 = _s.pop().val();
        _s.push(a1 < a2 ? 1 : 0);
    } else if (poppedCmd == GT) {
        CellValType a1 = _s.pop().val();
        CellValType a2 = _s.pop().val();
        _s.push(a1 > a2 ? 1 : 0);
    } else if (poppedCmd == CAR) {
        _s.push(_s.pop().car());
    } else if (poppedCmd == CDR) {
        _s.push(_s.pop().cdr());
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
                    out << "Compiled (" << code << ")";
                    out << " to ";
                    out << "(" << newc << ")";
                },
                Logger::DEBUG);
        newc.splice(_c);
        _c = newc;
    } else if (poppedCmd == LDG) {
        Handle newclosure = Handle::cons(_c.pop(), _e);

        Handle curName = _globals_names.car();
        for (int i = 0; i < _cur_global; i++) { curName = curName.cdr(); }
        _globals_names_map.emplace_back(newclosure, curName.car().strval());
        _cur_global++;

        _globals_vals.append(newclosure);
    } else if (poppedCmd == PRINT) {
        if (!_s.null()) {
            Handle val = _s.pop();
            bool cons = !val.atom();
            if (cons) _outstream << "(";
            _outstream << val;
            if (cons) _outstream << ")";
            _outstream << std::endl;
        }
    } else if (poppedCmd == READ) {
        std::string read;
        std::getline(_instream, read);
        Parser parser;
        parser.loadStr(read);
        _s.push(parser.parseExpr());
    } else {
        assert(false);
    }


    Logger::log(
            "VM",
            [&](std::ostream &out) {
                out << "Executed " << cmd_to_str.at(poppedCmd) << "\n";
                if (!_s.null()) out << "s:" << _s << "\n";
                if (!_e.null()) out << "e:" << _e << "\n";
                if (!_c.null()) out << "c:" << _c << "\n";
                if (!_d.null()) out << "d:" << _d << "\n";
            },
            Logger::TRACE);
}
