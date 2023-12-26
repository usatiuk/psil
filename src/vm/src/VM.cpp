//
// Created by Stepan Usatiuk on 22.12.2023.
//

#include <iostream>
#include <set>
#include <utility>

#include "Command.h"
#include "ConsUtils.h"
#include "VM.h"

using namespace ConsUtils;

VM::VM(std::istream &instream, std::ostream &outstream) : _instream(instream), _outstream(outstream) {}

void VM::run() {
    while (!_stop) step();
}

void VM::step() {
    MCHandle poppedH = pop(_c);

    switch (Command::int_to_cmd(val(poppedH))) {
        case Command::CommandNum::NIL: {
            push(_s, nullptr);
            break;
        }
        case Command::CommandNum::LDC: {
            push(_s, pop(_c));
            break;
        }
        case Command::CommandNum::LD: {
            MCHandle poppedH2 = pop(_c);

            int64_t frame = val(car(poppedH2));
            int64_t arg = val(cdr(poppedH2));

            assert(frame > 0);
            assert(arg > 0);

            MCHandle curFrame = _e;

            for (int i = 1; i < frame; i++) {
                curFrame = cdr(curFrame);
            }

            MCHandle curArg = car(curFrame);

            for (int i = 1; i < arg; i++) {
                curArg = cdr(curArg);
            }

            push(_s, car(curArg));
            break;
        }
        case Command::CommandNum::SEL: {
            MCHandle popped2H = pop(_s);

            MCHandle ct = pop(_c);
            MCHandle cf = pop(_c);

            push(_d, _c);
            if (val(popped2H) > 0) {
                _c = ct;
            } else {
                _c = cf;
            }

            break;
        }
        case Command::CommandNum::JOIN: {
            _c = pop(_d);
            break;
        }
        case Command::CommandNum::LDF: {
            push(_s, cons(pop(_c), _e));
            break;
        }
        case Command::CommandNum::AP: {
            MCHandle closureH = pop(_s);
            MCHandle argsH = pop(_s);

            push(_d, _s);
            push(_d, _e);
            push(_d, _c);

            _s = cons(nullptr, nullptr);
            _c = car(closureH);
            _e = cdr(closureH);
            push(_e, argsH);
            break;
        }
        case Command::CommandNum::RET: {
            MCHandle c = pop(_d);
            MCHandle e = pop(_d);
            MCHandle s = pop(_d);

            MCHandle ret = pop(_s);

            _c = c;
            _e = e;
            _s = s;

            push(_s, ret);
            //            gc();
            break;
        }
        case Command::CommandNum::DUM: {
            push(_e, nullptr);
            break;
        }
        case Command::CommandNum::RAP: {
            MCHandle closureH = pop(_s);
            MCHandle argsH = pop(_s);


            MCHandle origE = cdr(_e);

            push(_d, _s);
            push(_d, origE);
            push(_d, _c);

            _s = cons(nullptr, nullptr);
            _c = car(closureH);
            _e = cdr(closureH);

            MCHandle fnEnv = cdr(closureH);
            assert(_e.get() == fnEnv.get());

            push(_e, argsH);
            setcar(fnEnv, argsH);
            break;
        }
        case Command::CommandNum::STOP: {
            _stop = true;
            //            gc();
            break;
        }
        case Command::CommandNum::ADD: {
            int64_t ret = val(pop(_s)) + val(pop(_s));
            push(_s, makeIntCell(ret));
            break;
        }
        case Command::CommandNum::SUB: {
            break;
        }
        case Command::CommandNum::CONS: {
            MCHandle h1 = pop(_s);
            MCHandle h2 = pop(_s);

            push(_s, cons(h1, h2));
            break;
        }
        case Command::CommandNum::READCHAR: {
            char c;
            _instream >> c;
            push(_s, makeIntCell(c));
            break;
        }
        case Command::CommandNum::PUTCHAR: {
            _outstream << (char) val(pop(_s));
            break;
        }
        case Command::CommandNum::PUTNUM: {
            _outstream << val(pop(_s));
            break;
        }
        case Command::CommandNum::END: {
            assert(false);
            break;
        }
        default:
            assert(false);
    }
}

//void VM::gc() {
//    std::function<void(ConsCell *)> visit = [&](ConsCell *c) {
//        if (c == nullptr) return;
//        if (c->live) return;
//
//        c->live = true;
//
//        if (c->_car) {
//            if (c->_car->_type == CellType::CONS) visit(dynamic_cast<ConsCell *>(c->_car));
//            c->_car->live = true;
//        }
//        if (c->_cdr) {
//            if (c->_cdr->_type == CellType::CONS) visit(dynamic_cast<ConsCell *>(c->_cdr));
//            c->_cdr->live = true;
//        }
//    };
//    visit(_s);
//    visit(_e);
//    visit(_c);
//    visit(_d);
//
//    uint64_t freed = 0;
//
//    _cells.remove_if([&](Cell *l) {
//        bool ret = !l->live;
//        if (ret) {
//            freed += 1;
//            delete l;
//        } else {
//            l->live = false;
//        }
//        return ret;
//    });
//
//    std::cout << "GC Freed " << freed << std::endl;
//}
//