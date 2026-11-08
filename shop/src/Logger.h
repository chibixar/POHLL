#pragma once
#include <string>

class Logger
{
public:
    static void set_file(const std::string& path);
    static void set_role(const std::string& role);
    static void log(const std::string& action);
};
