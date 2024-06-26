//
// Created by Stepan Usatiuk on 22.12.2023.
//

#ifndef PSIL_VM_H
#define PSIL_VM_H

#include <iostream>
#include <list>
#include <utility>
#include <vector>

#include "Cell.h"
#include "MemoryContext.h"

class VM {
public:
    VM(std::istream &instream = std::cin, std::ostream &outstream = std::cout);

    void run();

    void loadControl(const Handle &h) {
        _c    = h;
        _stop = false;
    }

    void loadStack(const Handle &h) { _s = h; }

    bool is_quit() const { return _quit; }
    void step();

private:
    Handle                                  _globals_names = Handle::cons(Handle::cons(nullptr, nullptr), nullptr);
    Handle                                  _globals_vals  = Handle::cons(nullptr, nullptr);
    std::unordered_map<Handle, std::string> _globals_names_map;
    size_t                                  _cur_global     = 0;
    size_t                                  _cur_call_level = 0;
    Handle                                  _s              = Handle::cons(nullptr, nullptr);
    Handle                                  _e              = Handle::cons(_globals_vals, nullptr);
    Handle                                  _c              = Handle::cons(nullptr, nullptr);
    Handle                                  _d              = Handle::cons(nullptr, nullptr);
    bool                                    _stop           = false;
    bool                                    _quit           = false;

    std::istream &_instream;
    std::ostream &_outstream;
};

#endif //PSIL_VM_H
