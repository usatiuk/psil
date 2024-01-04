//
// Created by stepus53 on 3.1.24.
//

#ifndef PSIL_OPTIONS_H
#define PSIL_OPTIONS_H


#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

class Options {
public:
    template<typename T>
    static T get(const std::string &opt) {
        Options &o = get();
        std::shared_lock l(o._mutex);
        if (_defaults.find(opt) == _defaults.end()) throw std::invalid_argument("Unknown option " + opt);
        if (!std::holds_alternative<T>(_defaults.at(opt))) throw std::invalid_argument("Bad option type " + opt);
        return std::get<T>(o._current.at(opt));
    }

    template<typename T>
    static void set(const std::string &opt, const T &val) {
        Options &o = get();
        std::lock_guard l(o._mutex);
        if (_defaults.find(opt) == _defaults.end()) throw std::invalid_argument("Unknown option " + opt);
        if (!std::holds_alternative<T>(_defaults.at(opt))) throw std::invalid_argument("Bad option type " + opt);
        o._current[opt] = val;
    }

    static void reset();
    static Options &get();

private:
    const static inline std::unordered_map<std::string, std::variant<size_t, bool>> _defaults{{"cell_limit", 50000U},
                                                                                              {"command_strs", false},
                                                                                              {"default_log_level", 1U},
                                                                                              {"gc_threshold", 50U},
                                                                                              {"repl", true}};

    std::unordered_map<std::string, std::variant<size_t, bool>> _current = _defaults;
    std::shared_mutex _mutex;
};


#endif//PSIL_OPTIONS_H
