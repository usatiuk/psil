//
// Created by Stepan Usatiuk on 22.12.2023.
//

#include <iostream>
#include <set>
#include <utility>

#include "Command.h"
#include "ConsUtils.h"
#include "VM.h"

VM::VM(std::istream &instream, std::ostream &outstream) : _instream(instream), _outstream(outstream) {}

void VM::run() {
    while (!_stop) step();
}

void VM::step() {
    MCHandle poppedH = ConsUtils::pop(_c);
    ValueCell &popped = dynamic_cast<ValueCell &>(*poppedH);

    switch (Command::int_to_cmd(popped._val)) {
        case Command::CommandNum::NIL: {
            ConsUtils::push(_s, nullptr);
            break;
        }
        case Command::CommandNum::LDC: {
            ConsUtils::push(_s, ConsUtils::pop(_c));
            break;
        }
        case Command::CommandNum::LD: {
            MCHandle poppedH2 = ConsUtils::pop(_c);
            ConsCell &popped2 = dynamic_cast<ConsCell &>(*poppedH2);

            int64_t frame = dynamic_cast<ValueCell &>(*popped2._car)._val;
            int64_t arg = dynamic_cast<ValueCell &>(*popped2._cdr)._val;

            assert(frame > 0);
            assert(arg > 0);

            ConsCell *curFrame = dynamic_cast<ConsCell *>(_e.get());
            assert(curFrame);

            for (int i = 1; i < frame; i++) {
                curFrame = dynamic_cast<ConsCell *>(curFrame->_cdr);
                assert(curFrame);
            }

            ConsCell *curArg = dynamic_cast<ConsCell *>(curFrame->_car);
            assert(curArg);

            for (int i = 1; i < arg; i++) {
                curArg = dynamic_cast<ConsCell *>(curArg->_cdr);
                assert(curArg);
            }

            ConsUtils::push(_s, curArg->_car);
            break;
        }
        case Command::CommandNum::SEL: {
            MCHandle popped2H = ConsUtils::pop(_s);
            ValueCell &popped2 = dynamic_cast<ValueCell &>(*popped2H);

            MCHandle ct = ConsUtils::pop(_c);
            MCHandle cf = ConsUtils::pop(_c);

            ConsUtils::push(_d, _c);
            if (popped2._val > 0) {
                _c = ct;
            } else {
                _c = cf;
            }

            break;
        }
        case Command::CommandNum::JOIN: {
            _c = ConsUtils::pop(_d);
            break;
        }
        case Command::CommandNum::LDF: {
            ConsUtils::push(_s, ConsUtils::cons(ConsUtils::pop(_c), _e));
            break;
        }
        case Command::CommandNum::AP: {
            MCHandle closureH = ConsUtils::pop(_s);
            MCHandle argsH = ConsUtils::pop(_s);

            ConsUtils::push(_d, _s);
            ConsUtils::push(_d, _e);
            ConsUtils::push(_d, _c);

            _s = ConsUtils::cons(nullptr, nullptr);
            _c = ConsUtils::car(closureH);
            _e = ConsUtils::cdr(closureH);
            ConsUtils::push(_e, argsH);
            break;
        }
        case Command::CommandNum::RET: {
            MCHandle c = ConsUtils::pop(_d);
            MCHandle e = ConsUtils::pop(_d);
            MCHandle s = ConsUtils::pop(_d);

            MCHandle ret = ConsUtils::pop(_s);

            _c = c;
            _e = e;
            _s = s;

            ConsUtils::push(_s, ret);
            //            gc();
            break;
        }
        case Command::CommandNum::DUM: {
            ConsUtils::push(_e, nullptr);
            break;
        }
        case Command::CommandNum::RAP: {
            MCHandle closureH = ConsUtils::pop(_s);
            MCHandle argsH = ConsUtils::pop(_s);


            MCHandle origE = ConsUtils::cdr(_e);

            ConsUtils::push(_d, _s);
            ConsUtils::push(_d, origE);
            ConsUtils::push(_d, _c);

            _s = ConsUtils::cons(nullptr, nullptr);
            _c = ConsUtils::car(closureH);
            _e = ConsUtils::cdr(closureH);

            MCHandle fnEnv = ConsUtils::cdr(closureH);
            assert(_e.get() == fnEnv.get());
            ConsUtils::push(_e, argsH);
            dynamic_cast<ConsCell &>(*fnEnv)._car = argsH.get();
            break;
        }
        case Command::CommandNum::STOP: {
            _stop = true;
            //            gc();
            break;
        }
        case Command::CommandNum::ADD: {
            int64_t ret = dynamic_cast<ValueCell &>(*ConsUtils::pop(_s))._val + dynamic_cast<ValueCell &>(*ConsUtils::pop(_s))._val;
            ConsUtils::push(_s, CURRENT_MC.load()->create_cell<ValueCell>(ret));
            break;
        }
        case Command::CommandNum::SUB: {
            break;
        }
        case Command::CommandNum::CONS: {
            MCHandle h1 = ConsUtils::pop(_s);
            MCHandle h2 = ConsUtils::pop(_s);

            ConsUtils::push(_s, ConsUtils::cons(h1, h2));
            break;
        }
        case Command::CommandNum::READCHAR: {
            char c;
            _instream >> c;
            ConsUtils::push(_s, CURRENT_MC.load()->create_cell<ValueCell>(c));
            break;
        }
        case Command::CommandNum::PUTCHAR: {
            _outstream << (char) dynamic_cast<ValueCell &>(*ConsUtils::pop(_s))._val;
            break;
        }
        case Command::CommandNum::PUTNUM: {
            _outstream << dynamic_cast<ValueCell &>(*ConsUtils::pop(_s))._val;
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