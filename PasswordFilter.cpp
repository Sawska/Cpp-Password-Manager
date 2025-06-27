#include "PasswordFilter.h"

PasswordFilter::PasswordFilter(const std::map<std::string, std::string>& passwords)
    : passwords(passwords) // Use member initializer list
{
}

std::map<std::string, std::string> PasswordFilter::filter_by_website(const std::string& keyword) const {
    std::map<std::string, std::string> result;
    for (const auto& [website, password] : passwords) {
        if (website.find(keyword) != std::string::npos) {
            result[website] = password;
        }
    }
    return result;
}
