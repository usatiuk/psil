//
// Created by stepus53 on 3.1.24.
//

#include "Options.h"

#include <stdexcept>

Options &Options::get() {
    static Options opts;
    return opts;
}

void Options::reset() { get()._current = _defaults; }
