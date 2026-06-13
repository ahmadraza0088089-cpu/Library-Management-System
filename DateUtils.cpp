#include "DateUtils.hpp"

#include <stdexcept>

namespace lms {

namespace {

std::tm toLocalTm(const std::chrono::system_clock::time_point& tp) {
    const std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm localTime{};
#if defined(_MSC_VER)
    localtime_s(&localTime, &time);
#else
    const std::tm* result = std::localtime(&time);
    if (result != nullptr) {
        localTime = *result;
    }
#endif
    return localTime;
}

}  // namespace

Date::Date() : daysSinceEpoch_(toDays(std::chrono::system_clock::now())) {}

Date::Date(const std::chrono::system_clock::time_point& tp) : daysSinceEpoch_(toDays(tp)) {}

Date Date::today() {
    return Date{};
}

Date Date::fromDaysSinceEpoch(int days) {
    Date date;
    date.daysSinceEpoch_ = days;
    return date;
}

Date Date::addDays(int days) const {
    return fromDaysSinceEpoch(daysSinceEpoch_ + days);
}

int Date::daysBetween(const Date& other) const {
    return daysSinceEpoch_ - other.daysSinceEpoch_;
}

std::string Date::toString() const {
    const auto tp = std::chrono::system_clock::from_time_t(
        static_cast<std::time_t>(daysSinceEpoch_) * 24 * 60 * 60);
    const std::tm localTime = toLocalTm(tp);

    std::ostringstream stream;
    stream << std::put_time(&localTime, "%Y-%m-%d");
    return stream.str();
}

int Date::toDaysSinceEpoch() const {
    return daysSinceEpoch_;
}

bool Date::operator<(const Date& other) const {
    return daysSinceEpoch_ < other.daysSinceEpoch_;
}

bool Date::operator<=(const Date& other) const {
    return daysSinceEpoch_ <= other.daysSinceEpoch_;
}

bool Date::operator>(const Date& other) const {
    return daysSinceEpoch_ > other.daysSinceEpoch_;
}

bool Date::operator==(const Date& other) const {
    return daysSinceEpoch_ == other.daysSinceEpoch_;
}

int Date::toDays(const std::chrono::system_clock::time_point& tp) {
    const std::tm localTime = toLocalTm(tp);
    std::tm midnight = localTime;
    midnight.tm_hour = 0;
    midnight.tm_min = 0;
    midnight.tm_sec = 0;

    const std::time_t midnightTime = std::mktime(&midnight);
    if (midnightTime == static_cast<std::time_t>(-1)) {
        throw std::runtime_error("Failed to convert date to epoch days.");
    }

    return static_cast<int>(midnightTime / (24 * 60 * 60));
}

}  // namespace lms
