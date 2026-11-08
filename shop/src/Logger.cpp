#include "Logger.h"
#include <ctime>
#include <fstream>
#include <iomanip>

namespace
{
std::string g_path = "logs.txt";
std::string g_role = "System";
}

void Logger::set_file(const std::string& path) { g_path = path; }
void Logger::set_role(const std::string& role) { g_role = role; }

void Logger::log(const std::string& action)
{
    std::ofstream out(g_path, std::ios::app);
    if (!out)
        return;

    std::time_t now = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    out << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " | " << std::left << std::setw(14)
        << g_role << " | " << action << '\n';
}
