//
// Created by stepus53 on 3.1.24.
//

#ifndef PSIL_LOGGER_H
#define PSIL_LOGGER_H

#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <ostream>
#include <shared_mutex>
#include <string>
#include <unordered_map>

class Logger {
public:
    enum LogLevel { ALWAYS = 0, ERROR = 1, INFO = 2, DEBUG = 3, TRACE = 4 };
    static void log(const std::string &tag, const std::string &what, int level);
    static void log(const std::string &tag, const std::function<void(std::ostream &)> &fn, int level);

    // 0 - disabled
    // 1 - error
    // 2 - info
    // 3 - debug
    // 4 - trace
    static void set_level(const std::string &tag, int level);

    static void set_out(std::ostream &out);
    static void set_out_err(std::ostream &out_err);

    static void reset();

    static Logger &get();

private:
    std::unordered_map<std::string, int> _levels;
    static inline std::unordered_map<int, std::string> _level_names{
            {ALWAYS, "ALWAYS"}, {ERROR, "ERROR"}, {INFO, "INFO"}, {DEBUG, "DEBUG"}, {TRACE, "TRACE"},
    };
    std::chrono::time_point<std::chrono::high_resolution_clock> _start_time = std::chrono::high_resolution_clock::now();
    std::reference_wrapper<std::ostream> _out = std::cout;
    std::reference_wrapper<std::ostream> _out_err = std::cerr;
    std::shared_mutex _mutex;
};


#endif//PSIL_LOGGER_H
