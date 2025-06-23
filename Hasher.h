#ifndef HASHER_H
#define HASHER_H

#include <string>

class Hasher {
public:
    std::string hash_password(const std::string& password);
    bool verify_password(const std::string& password, const std::string& hash);
};

#endif // HASHER_H
