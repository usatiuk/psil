//
// Created by stepus53 on 3.1.24.
//

#include "Options.h"

#include <stdexcept>

Options &Options::get() {
    static Options opts;
    return opts;
}

void Options::reset() {
    std::lock_guard l(get()._mutex);
    get()._current = _defaults;
}
