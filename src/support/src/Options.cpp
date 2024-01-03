//
// Created by stepus53 on 3.1.24.
//

#include "Options.h"

#include <stdexcept>

Options &Options::get() {
    static Options opts;
    return opts;
}
bool Options::get_bool(const std::string &opt) {
    Options &o = get();
    std::shared_lock l(o._mutex);
    if (_defaults.find(opt) == _defaults.end()) throw std::invalid_argument("Unknown option " + opt);
    if (!std::holds_alternative<bool>(_defaults.at(opt))) throw std::invalid_argument("Bad option type " + opt);
    return std::get<bool>(o._current.at(opt));
}
size_t Options::get_int(const std::string &opt) {
    Options &o = get();
    std::shared_lock l(o._mutex);
    if (_defaults.find(opt) == _defaults.end()) throw std::invalid_argument("Unknown option " + opt);
    if (!std::holds_alternative<size_t>(_defaults.at(opt))) throw std::invalid_argument("Bad option type " + opt);
    return std::get<size_t>(o._current.at(opt));
}
void Options::set_bool(const std::string &opt, bool val) {
    Options &o = get();
    std::lock_guard l(o._mutex);
    if (_defaults.find(opt) == _defaults.end()) throw std::invalid_argument("Unknown option " + opt);
    if (!std::holds_alternative<bool>(_defaults.at(opt))) throw std::invalid_argument("Bad option type " + opt);
    o._current[opt] = val;
}
void Options::set_int(const std::string &opt, size_t val) {
    Options &o = get();
    std::lock_guard l(o._mutex);
    if (_defaults.find(opt) == _defaults.end()) throw std::invalid_argument("Unknown option " + opt);
    if (!std::holds_alternative<size_t>(_defaults.at(opt))) throw std::invalid_argument("Bad option type " + opt);
    o._current[opt] = val;
}
void Options::reset() {
    std::lock_guard l(get()._mutex);
    get()._current = _defaults;
}
