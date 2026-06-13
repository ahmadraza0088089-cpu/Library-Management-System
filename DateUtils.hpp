#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace lms {

class Date {
public:
    Date();
    explicit Date(const std::chrono::system_clock::time_point& tp);

    static Date today();
    static Date fromDaysSinceEpoch(int days);

    Date addDays(int days) const;
    int daysBetween(const Date& other) const;

    std::string toString() const;
    int toDaysSinceEpoch() const;

    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator==(const Date& other) const;

private:
    int daysSinceEpoch_;

    static int toDays(const std::chrono::system_clock::time_point& tp);
};

}  // namespace lms
