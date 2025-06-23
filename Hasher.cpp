#include "Hasher.h"
#include <argon2.h>
#include <stdexcept>
#include <cstring>

std::string Hasher::hash_password(const std::string& password) {
    char hash[128];

    int result = argon2_hash(
        2,                // iterations
        1 << 16,          // memory (64MB)
        1,                // parallelism
        password.c_str(), password.size(),
        "somesalt", 8,    // static salt (use a real random salt in production)
        hash, sizeof(hash),
        NULL, 0,
        Argon2_id         // Argon2id variant
    );

    if (result != ARGON2_OK)
        throw std::runtime_error("Argon2 hashing failed");

    return std::string(hash);
}

bool Hasher::verify_password(const std::string& password, const std::string& hash) {
    int result = argon2_verify(
        hash.c_str(),             // encoded hash
        password.c_str(), password.size(),
        Argon2_id
    );
    return result == ARGON2_OK;
}
