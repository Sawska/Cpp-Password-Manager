#ifndef PASSWORD_FILTER_H
#define PASSWORD_FILTER_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>

class PasswordFilter {
public:
        PasswordFilter(const std::map<std::string, std::string>& passwords);
    
    std::map<std::string, std::string> filter_by_website(const std::string& keyword) const;


private:
    std::map<std::string, std::string> passwords;
};

#endif // PASSWORD_FILTER_H
