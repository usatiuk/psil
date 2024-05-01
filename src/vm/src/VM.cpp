//
// Created by Stepan Usatiuk on 22.12.2023.
//

#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
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
    Handle      poppedH   = _c.pop();
    CellValType poppedCmd = poppedH.type() == CellType::STRATOM ? str_to_cmd.at(poppedH.strval()) : poppedH.val();

    Logger::log(
            Logger::VM,
            [&](std::ostream &out) {
                out << "Executing " << cmd_to_str.at(poppedCmd) << "\n";
                if (!_s.null()) out << "s:" << _s << "\n";
                if (!_e.null()) out << "e:" << _e << "\n";
                if (!_c.null()) out << "c:" << _c << "\n";
                if (!_d.null()) out << "d:" << _d << "\n";
            },
            Logger::TRACE);

    switch (poppedCmd) {
        case NIL: {
            _s.push(nullptr);
            break;
        }
        case LDC: {
            _s.push(_c.pop());
            break;
        }
        case ATOM: {
            _s.push(_s.pop().atom() ? 1 : 0);
            break;
        }
        case NILC: {
            _s.push(_s.pop().null() ? 1 : 0);
            break;
        }
        case LD: {
            Handle poppedH2 = _c.pop();

            int64_t frame = poppedH2.car().val();
            int64_t arg   = poppedH2.cdr().val();

            assert(frame > 0);
            assert(arg > 0);

            Handle curFrame = _e;

            for (int i = 1; i < frame; i++) { curFrame = curFrame.cdr(); }

            Handle curArg = curFrame.car();

            for (int i = 1; i < arg; i++) { curArg = curArg.cdr(); }

            _s.push(curArg.car());
            break;
        }
        case SEL: {
            Handle popped2H = _s.pop();

            Handle ct = _c.pop();
            Handle cf = _c.pop();

            _d.push(_c);
            if (popped2H.val() > 0) {
                _c = ct;
            } else {
                _c = cf;
            }
            break;
        }
        case JOIN: {
            _c = _d.pop();
            break;
        }
        case LDF: {
            _s.push(Handle::cons(_c.pop(), _e));
            break;
        }
        case AP: {
            Handle closureH = _s.pop();
            Handle argsH    = _s.pop();

            _d.push(_s);
            _d.push(_e);
            _d.push(_c);

            std::optional<std::string> name;
            if (Logger::en_level(Logger::VM, Logger::DEBUG)) {
                name = "unknown";
                if (_globals_names_map.find(closureH) != _globals_names_map.end())
                    name = _globals_names_map.at(closureH);
                _d.push(Handle(*name));
            }

            _s = Handle::cons(nullptr, nullptr);
            _c = closureH.car();
            _e = closureH.cdr();

            Logger::log(
                    Logger::VM,
                    [&](std::ostream &out) {
                        out << _cur_call_level;
                        for (int i = 0; i < _cur_call_level; i++) out << "  ";
                        out << " Applying " << *name;
                        out << " with args " << argsH;
                    },
                    Logger::DEBUG);

            _cur_call_level++;
            _e.push(argsH);
            break;
        }
        case RET: {
            _cur_call_level--;

            Handle n;
            if (Logger::en_level(Logger::VM, Logger::DEBUG)) n = _d.pop();

            Handle c = _d.pop();
            Handle e = _d.pop();
            Handle s = _d.pop();

            Handle ret = _s.pop();

            _c = c;
            _e = e;
            _s = s;

            Logger::log(
                    Logger::VM,
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
            break;
        }
        case DUM: {
            _e.push(nullptr);
            break;
        }
        case RAP: {
            Handle closureH = _s.pop();
            Handle argsH    = _s.pop();

            Handle origE = _e.cdr();

            _d.push(_s);
            _d.push(origE);
            _d.push(_c);

            if (Logger::en_level(Logger::VM, Logger::DEBUG)) _d.push(Handle("rap"));

            _s = Handle::cons(nullptr, nullptr);
            _c = closureH.car();
            _e = closureH.cdr();

            Handle fnEnv = closureH.cdr();
            assert(_e == fnEnv);

            _cur_call_level++;

            _e.push(argsH);
            fnEnv.setcar(argsH);
            break;
        }
        case STOP: {
            _stop = true;
            break;
        }
        case QUIT: {
            _stop = true;
            _quit = true;
            break;
        }
        case ADD: {
            _s.push(_s.pop().val() + _s.pop().val());
            break;
        }
        case MULT: {
            _s.push(_s.pop().val() * _s.pop().val());
            break;
        }
        case SUB: {
            CellValType a1 = _s.pop().val();
            CellValType a2 = _s.pop().val();
            _s.push(a1 - a2);
            break;
        }
        case DIV: {
            CellValType a1 = _s.pop().val();
            CellValType a2 = _s.pop().val();
            _s.push(a1 / a2);
            break;
        }
        case EQ: {
            _s.push(_s.pop() == _s.pop() ? 1 : 0);
            break;
        }
        case LT: {
            CellValType a1 = _s.pop().val();
            CellValType a2 = _s.pop().val();
            _s.push(a1 < a2 ? 1 : 0);
            break;
        }
        case GT: {
            CellValType a1 = _s.pop().val();
            CellValType a2 = _s.pop().val();
            _s.push(a1 > a2 ? 1 : 0);
            break;
        }
        case CAR: {
            _s.push(_s.pop().car());
            break;
        }
        case CDR: {
            _s.push(_s.pop().cdr());
            break;
        }
        case CONS: {
            Handle h1 = _s.pop();
            Handle h2 = _s.pop();

            _s.push(Handle::cons(h1, h2));
            break;
        }
        case READCHAR: {
            char c;
            _instream >> c;
            _s.push(Handle::makeNumCell(c));
            break;
        }
        case PUTCHAR: {
            _outstream << (char) _s.pop().val();
            break;
        }
        case PUTNUM: {
            _outstream << _s.pop().val();
            break;
        }
        case EVAL: {
            Handle code = _s.pop();
            Handle newc = Compiler::compile(code, _globals_names);
            Logger::log(
                    Logger::Compiler,
                    [&](std::ostream &out) {
                        out << "Compiled (" << code << ")";
                        out << " to ";
                        out << "(" << newc << ")";
                    },
                    Logger::DEBUG);
            newc.splice(_c);
            _c = newc;
            break;
        }
        case LDG: {
            Handle newclosure = Handle::cons(_c.pop(), _e);

            Handle curName = _globals_names.car();
            for (int i = 0; i < _cur_global; i++) { curName = curName.cdr(); }
            _globals_names_map.emplace(newclosure, curName.car().strval());
            _cur_global++;

            _globals_vals.append(newclosure);
            break;
        }
        case PRINT: {
            if (!_s.null()) {
                Handle val  = _s.pop();
                bool   cons = !val.atom();
                if (cons) _outstream << "(";
                _outstream << val;
                if (cons) _outstream << ")";
                _outstream << std::endl;
            }
            break;
        }
        case READ: {
            std::string read;
            std::getline(_instream, read);
            _s.push(Parser::parse_str(read));
            break;
        }
        default:
            throw std::invalid_argument("Unknown command: " + std::to_string(poppedCmd));
    }

    Logger::log(
            Logger::VM,
            [&](std::ostream &out) {
                out << "Executed " << cmd_to_str.at(poppedCmd) << "\n";
                if (!_s.null()) out << "s:" << _s << "\n";
                if (!_e.null()) out << "e:" << _e << "\n";
                if (!_c.null()) out << "c:" << _c << "\n";
                if (!_d.null()) out << "d:" << _d << "\n";
            },
            Logger::TRACE);
}
