#pragma once
#include <string_view>

namespace UI {
    inline constexpr std::string_view RESET   = "\033[0m";
    inline constexpr std::string_view BOLD    = "\033[1m";
    inline constexpr std::string_view RED     = "\033[31m";
    inline constexpr std::string_view GREEN   = "\033[32m";
    inline constexpr std::string_view YELLOW  = "\033[33m";
    inline constexpr std::string_view BLUE    = "\033[34m";
    inline constexpr std::string_view MAGENTA = "\033[35m";
    inline constexpr std::string_view CYAN    = "\033[36m";
    inline constexpr std::string_view WHITE   = "\033[37m";
    inline constexpr std::string_view GRAY    = "\033[90m";
}

inline constexpr int MENU_EXIT = 0;
inline constexpr int MENU_MAX  = 10;

inline constexpr int ARTICLE_LEN = 16;
inline constexpr int NAME_LEN    = 32;

inline constexpr double MIN_PRICE = 0.01;
inline constexpr double MAX_PRICE = 1'000'000.0;
inline constexpr int MIN_QUANTITY = 0;
inline constexpr int MAX_QUANTITY = 100'000;

inline constexpr int MIN_DPI      = 100, MAX_DPI      = 100'000;
inline constexpr int MIN_REFRESH  = 30,  MAX_REFRESH  = 1000;
inline constexpr int MIN_TDP      = 1,   MAX_TDP      = 1000;
inline constexpr int MIN_CLOCK    = 500, MAX_CLOCK    = 8000;
inline constexpr int MIN_CAPACITY = 1,   MAX_CAPACITY = 1024;

inline constexpr int CAT_MOUSE     = 1;
inline constexpr int CAT_KEYBOARD  = 2;
inline constexpr int CAT_MONITOR   = 3;
inline constexpr int CAT_PROCESSOR = 4;
inline constexpr int CAT_RAM       = 5;

inline constexpr int LOW_STOCK_THRESHOLD = 5;
