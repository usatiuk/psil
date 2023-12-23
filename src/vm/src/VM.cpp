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
            IntCell *popped2 = dynamic_cast<IntCell *>(pop(_c));
            push(_s, popped2);
            break;
        }
        case CommandCell::CommandNum::LD: {
            ConsCell *popped2 = dynamic_cast<ConsCell *>(pop(_c));

            int64_t frame = dynamic_cast<IntCell *>(popped2->_car)->_val;
            int64_t arg = dynamic_cast<IntCell *>(popped2->_cdr)->_val;
            // todo

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
            break;
        }
        case CommandCell::CommandNum::AP: {
            break;
        }
        case CommandCell::CommandNum::RET: {
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
            break;
        }
        case CommandCell::CommandNum::SUB: {
            break;
        }
        case CommandCell::CommandNum::END: {
            break;
        }
        case CommandCell::CommandNum::READCHAR:
            char c;
            _instream >> c;
            push(_s, makeCell<IntCell>(c));
            break;
        case CommandCell::CommandNum::PUTCHAR:
            IntCell *popped2 = dynamic_cast<IntCell *>(pop(_s));
            _outstream << (char) popped2->_val;
            break;
    }

}

VM::VM(std::istream &instream, std::ostream &outstream) : _instream(instream), _outstream(outstream) {
    _s = dynamic_cast<ConsCell *>(makeCell<ConsCell>(nullptr));
    _e = dynamic_cast<ConsCell *>(makeCell<ConsCell>(nullptr));
    _c = dynamic_cast<ConsCell *>(makeCell<ConsCell>(nullptr));
    _d = dynamic_cast<ConsCell *>(makeCell<ConsCell>(nullptr));
}
