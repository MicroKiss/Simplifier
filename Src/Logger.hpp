#pragma once
#include <iostream>
#include <sstream>

class Logger {
  public:
    bool silent;

    Logger () : silent (false) {}

    // Log to stdout with newline
    template <typename... Args>
    void Log (Args &&...args) const {
        if (silent)
            return;

        std::ostringstream oss;
        (oss << ... << std::forward<Args> (args));
        std::cout << oss.str () << std::endl;
    }

    // Log to stderr with newline and red color
    template <typename... Args>
    void Error (Args &&...args) const {
        if (silent)
            return;

        std::ostringstream oss;
        (oss << ... << std::forward<Args> (args));
        std::cerr << "\033[1;31m" << oss.str () << "\033[0m" << std::endl;
    }
};