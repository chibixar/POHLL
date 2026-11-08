#pragma once
#include "exceptions.h"
#include <iomanip>
#include <sstream>
#include <string>

inline size_t utf8_length(const std::string& s)
{
    size_t n = 0;
    for (unsigned char c : s)
        if ((c & 0xC0) != 0x80)
            ++n;
    return n;
}

inline std::string trim(const std::string& s)
{
    const char* ws = " \t\r\n";
    size_t b = s.find_first_not_of(ws);
    if (b == std::string::npos)
        return "";
    size_t e = s.find_last_not_of(ws);
    return s.substr(b, e - b + 1);
}

inline void check_text(const std::string& v, const std::string& field, size_t max_len)
{
    if (v.empty())
        throw ValidationException(field + ": Value cannot be empty.");
    if (utf8_length(v) > max_len)
        throw ValidationException(field + ": Exceeds max length of " + std::to_string(max_len) + " characters.");
    if (v.find(';') != std::string::npos)
        throw ValidationException(field + ": Semicolon (';') is not permitted.");
}

inline int parse_int(const std::string& raw, const std::string& field)
{
    std::string text = trim(raw);
    size_t pos = 0;
    int v = 0;
    try { v = std::stoi(text, &pos); }
    catch (...) { throw ValidationException(field + ": Expected integer number."); }
    if (pos != text.size())
        throw ValidationException(field + ": Unexpected trailing characters after integer.");
    return v;
}

inline double parse_double(const std::string& raw, const std::string& field)
{
    std::string text = trim(raw);
    size_t pos = 0;
    double v = 0;
    try { v = std::stod(text, &pos); }
    catch (...) { throw ValidationException(field + ": Expected numeric decimal."); }
    if (pos != text.size())
        throw ValidationException(field + ": Unexpected trailing characters after number.");
    return v;
}

template <typename T>
void check_range(T v, T lo, T hi, const std::string& field)
{
    if (!(v >= lo && v <= hi))
    {
        std::ostringstream ss;
        ss << field << ": Value must be between " << lo << " and " << hi << ".";
        throw ValidationException(ss.str());
    }
}

inline std::string format_double(double v, int precision)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << v;
    return ss.str();
}
