#ifndef UTILL_H_
#define UTILL_H_

#include <map>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <chrono>
#include <numeric>
#include <iomanip>

mongocxx::instance inst{};
mongocxx::client conn{};

std::string dash_line = "---------------------------";
std::string shap_line = "###########################";


std::tuple<int, int, int, int, int, int> ExtractTimestampComponents(int64_t TIMESTAMP)
{
    std::time_t timestamp = static_cast<std::time_t>(TIMESTAMP);

    // Convert to UTC time structure
    std::tm *timeinfo = std::gmtime(&timestamp);

    // Extract components
    int year = timeinfo->tm_year + 1900;
    int month = timeinfo->tm_mon + 1;
    int day = timeinfo->tm_mday;
    int hour = timeinfo->tm_hour;
    int minute = timeinfo->tm_min;
    int second = timeinfo->tm_sec;

    // Return as a tuple
    return std::make_tuple(year, month, day, hour, minute, second);
}


#endif