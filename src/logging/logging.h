//
// Created by chris on 27/12/20.
//

#ifndef UNTITLED1_LOGGING_H
#define UNTITLED1_LOGGING_H

#include <string>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/color.h>

#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define RESET "\033[0m"

#define BOLD "\033[1m"
#define L_ERR(x) style(RED,x)
#define L_HLT(x) style(MAGENTA BOLD, x)
std::string style(const std::string& s, const std::string& str);

inline std::string E_FMT(const std::string& x) {
    return fmt::format("{}", x);
}

inline std::string E_HLT(const std::string& x) {
    if (getenv("NO_COLOR") == nullptr){
        return fmt::format("{}", x);
    }
    return fmt::format(fmt::fg(fmt::terminal_color::red) | fmt::emphasis::bold, "{}", x);
}

inline std::string E_INFO(const std::string& x) {
#ifdef NO_COLOR
    return fmt::format("{}", x);
#else
    return fmt::format(fmt::fg(fmt::terminal_color::blue), "{}", x);
#endif
}

#define E_LINE(x) fmt::format(fmt::fg(fmt::terminal_color::white),"{}", x)
#endif //UNTITLED1_LOGGING_H
