//
// Created by stepus53 on 3.1.24.
//

#include "Logger.h"

#include <iomanip>
#include <sstream>

#include "Options.h"
Logger &Logger::get() {
    static Logger logger;
    return logger;
}

void Logger::log(const std::string &tag, const std::string &what, int level) {
    if (!en_level(tag, level)) return;
    {
        std::shared_lock l(get()._mutex);
        auto now = std::chrono::high_resolution_clock::now();
        std::stringstream out;
        out << std::setprecision(4) << std::fixed << "["
            << static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(now - get()._start_time).count()) / 1000.0 << "s]"
            << "[" << tag << "][" << get()._level_names.at(level) << "] " << what << '\n';

        if (level == 1) get()._out_err.get() << out.str();
        else
            get()._out.get() << out.str();
    }
}

void Logger::log(const std::string &tag, const std::function<void(std::ostream &)> &fn, int level) {
    if (!en_level(tag, level)) return;

    std::stringstream out;
    fn(out);
    log(tag, out.str(), level);
}

void Logger::set_level(const std::string &tag, int level) {
    std::lock_guard l(get()._mutex);
    get()._levels[tag] = level;
}
void Logger::set_out(std::ostream &out) {
    std::lock_guard l(get()._mutex);
    get()._out = out;
}
void Logger::set_out_err(std::ostream &out_err) {
    std::lock_guard l(get()._mutex);
    get()._out_err = out_err;
}
void Logger::reset() {
    std::lock_guard l(get()._mutex);
    get()._levels = {};
}
int Logger::get_level(const std::string &tag) {
    std::shared_lock l(get()._mutex);
    int en_level = Options::get_int("default_log_level");
    if (get()._levels.find(tag) != get()._levels.end()) en_level = get()._levels.at(tag);
    return en_level;
}
bool Logger::en_level(const std::string &tag, int level) {
    int en_level = get_level(tag);
    if (en_level < level) return false;
    return true;
}
