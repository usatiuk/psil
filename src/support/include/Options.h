//
// Created by stepus53 on 3.1.24.
//

#ifndef PSIL_OPTIONS_H
#define PSIL_OPTIONS_H


#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <variant>
class Options {
public:
    static bool get_bool(const std::string &opt);
    static size_t get_int(const std::string &opt);
    static void set_bool(const std::string &opt, bool val);
    static void set_int(const std::string &opt, size_t val);
    static void reset();
    static Options &get();

private:
    const static inline std::unordered_map<std::string, std::variant<size_t, bool>> _defaults{{"cell_limit", 50000U},
                                                                                              {"command_strs", false},
                                                                                              {"default_log_level", 1U},
                                                                                              {"repl", true}};

    std::unordered_map<std::string, std::variant<size_t, bool>> _current = _defaults;
    std::shared_mutex _mutex;
};


#endif//PSIL_OPTIONS_H
