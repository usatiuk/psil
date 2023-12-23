//
// Created by Stepan Usatiuk on 22.12.2023.
//

#include "../includes/VM.h"

#include <utility>
#include <iostream>

void VM::run() {
    while (!_stop) step();
}

void VM::step() {
    CommandCell *popped = dynamic_cast<CommandCell *>(pop(_c));

    switch (popped->intToCmd()) {
        case CommandCell::CommandNum::NIL: {
            push(_s, nullptr);
            break;
        }
        case CommandCell::CommandNum::LDC: {
            push(_s, pop(_c));
            break;
        }
        case CommandCell::CommandNum::LD: {
            ConsCell *popped2 = dynamic_cast<ConsCell *>(pop(_c));

            int64_t frame = dynamic_cast<IntCell *>(popped2->_car)->_val;
            int64_t arg = dynamic_cast<IntCell *>(popped2->_cdr)->_val;

            assert(frame > 0);
            assert(arg > 0);

            ConsCell *curFrame = _e;

            for (int i = 1; i < frame; i++) {
                curFrame = dynamic_cast<ConsCell *>(_e->_cdr);
            }

            ConsCell *curArg = dynamic_cast<ConsCell *>(curFrame->_car);
            for (int i = 1; i < arg; i++) {
                curArg = dynamic_cast<ConsCell *>(curArg->_cdr);
            }

            push(_s, curArg->_car);

            break;
        }
        case CommandCell::CommandNum::SEL: {
            IntCell *popped2 = dynamic_cast<IntCell *>(pop(_s));
            ConsCell *ct = dynamic_cast<ConsCell *>(pop(_c));
            ConsCell *cf = dynamic_cast<ConsCell *>(pop(_c));
            ConsCell *ret = _c;
            push(_d, ret);
            if (popped2->_val > 0) {
                _c = ct;
            } else {
                _c = cf;
            }

            break;
        }
        case CommandCell::CommandNum::JOIN: {
            ConsCell *ret = dynamic_cast<ConsCell *>(pop(_d));
            _c = ret;
            break;
        }
        case CommandCell::CommandNum::LDF: {
            ConsCell *fn = dynamic_cast<ConsCell *>(pop(_c));
            push(_s, makeCell<ConsCell>(fn, _e));
            break;
        }
        case CommandCell::CommandNum::AP: {
            ConsCell *closure = dynamic_cast<ConsCell *>(pop(_s));
            ConsCell *args = dynamic_cast<ConsCell *>(pop(_s));

            push(_d, _s);
            push(_d, _e);
            push(_d, _c);

            _s = makeCell<ConsCell>();
            _c = dynamic_cast<ConsCell *>(closure->_car);
            _e = dynamic_cast<ConsCell *>(closure->_cdr);
            push(_e, args);
            break;
        }
        case CommandCell::CommandNum::RET: {
            ConsCell *c = dynamic_cast<ConsCell *>(pop(_d));
            ConsCell *e = dynamic_cast<ConsCell *>(pop(_d));
            ConsCell *s = dynamic_cast<ConsCell *>(pop(_d));

            Cell *ret = pop(_s);

            _c = c;
            _e = e;
            _s = s;

            push(_s, ret);

            break;
        }
        case CommandCell::CommandNum::DUM: {
            break;
        }
        case CommandCell::CommandNum::RAP: {
            break;
        }
        case CommandCell::CommandNum::STOP: {
            _stop = true;
            break;
        }
        case CommandCell::CommandNum::ADD: {
            IntCell *a1 = dynamic_cast<IntCell *>(pop(_s));
            IntCell *a2 = dynamic_cast<IntCell *>(pop(_s));
            push(_s, makeCell<IntCell>(a1->_val + a2->_val));
            break;
        }
        case CommandCell::CommandNum::SUB: {
            break;
        }
        case CommandCell::CommandNum::READCHAR: {
            char c;
            _instream >> c;
            push(_s, makeCell<IntCell>(c));
            break;
        }
        case CommandCell::CommandNum::PUTCHAR: {
            IntCell *popped2 = dynamic_cast<IntCell *>(pop(_s));
            _outstream << (char) popped2->_val;
            break;
        }
        case CommandCell::CommandNum::END: {
            assert(false);
            break;
        }
        default:
            assert(false);
    }

}

VM::VM(std::istream &instream, std::ostream &outstream) : _instream(instream), _outstream(outstream) {
    _s = dynamic_cast<ConsCell *>(makeCell<ConsCell>(nullptr));
    _e = dynamic_cast<ConsCell *>(makeCell<ConsCell>(nullptr));
    _c = dynamic_cast<ConsCell *>(makeCell<ConsCell>(nullptr));
    _d = dynamic_cast<ConsCell *>(makeCell<ConsCell>(nullptr));
}
