#include <fc/log/logger.hpp>

void UUOS::set_log_level(string& logger_name, int level) {
    fc::logger::get(logger_name).set_log_level(fc::log_level(level));
}

