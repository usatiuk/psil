//
// Created by Stepan Usatiuk on 22.12.2023.
//

#ifndef PSIL_VM_H
#define PSIL_VM_H

#include <vector>
#include<utility>
#include <iostream>

#include "Cell.h"

class VM {
public:
    VM(std::istream &instream = std::cin, std::ostream &outstream = std::cout);

    void run();

    void step();

//    template<typename T>
//    void appendCommand(T cell) {
//        push(_c, makeCell<T>(std::move(cell)));
//    }

    template<typename T>
    void appendCommand(T *cell) {
        push(_c, cell);
    }

    template<typename CT, typename... Args>
    CT *makeCell(Args... args) {
        return static_cast<CT *>(_cells.emplace_back(new CT(std::forward<Args>(args)...)));
    }

    Cell *car(ConsCell *cell) {
        return cell->_car;
    }

    Cell *cdr(ConsCell *cell) {
        return cell->_cdr;
    }

    ConsCell *cons(Cell *car, Cell *cdr) {
        return dynamic_cast<ConsCell *>(makeCell<ConsCell>(car, cdr));
    }

    Cell *pop(ConsCell *&what) {
        Cell *ret = what->_car;
        what = dynamic_cast<ConsCell *>(cdr(what));
        return ret;
    }

    Cell *push(ConsCell *&what, Cell *toAppend) {
        what = cons(toAppend, what);
        return what;
    }

private:
    std::vector<Cell *> _cells;
    ConsCell *_s = nullptr;
    ConsCell *_e = nullptr;
    ConsCell *_c = nullptr;
    ConsCell *_d = nullptr;
    bool _stop = false;

    std::istream &_instream;
    std::ostream &_outstream;
};


#endif //PSIL_VM_H
