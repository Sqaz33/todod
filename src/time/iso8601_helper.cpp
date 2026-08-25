#include "iso8601_helper.hpp"

#include <format>
#include <sstream>
#include <regex>

namespace todod::helpers {

std::string timePointToIso8601(std::chrono::system_clock::time_point tp) {
    return std::format("{:%FT%T%Z}", tp);
}

std::chrono::system_clock::time_point iso8601ToTimePoint(const std::string& str) {
    std::istringstream is{str};
    std::chrono::system_clock::time_point tp;
    is >> std::chrono::parse("%Y-%m-%dT%H:%M:%S%Z", tp);
    
    return tp;
}

bool isValidIso8601(const std::string& str) {
    std::regex pattern(
        R"iso6801(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{3}Z$)iso6801");
    return std::regex_match(str, pattern);
}

} // namespace todod::helpers