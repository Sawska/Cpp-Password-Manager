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
    "somesalt", 8,    // salt
    hash, sizeof(hash),
    NULL, 0,          // encoded, encodedlen
    Argon2_id,        // Argon2id variant
    ARGON2_VERSION_13 // version
);

    if (result != ARGON2_OK)
        throw std::runtime_error("Argon2 hashing failed");

    return std::string(hash);
}

bool Hasher::verify_password(const std::string& password, const std::string& hash) {
    int result = argon2_verify(
        hash.c_str(),           // encoded hash
        password.c_str(), password.size(),
        Argon2_id               // Argon2id variant
    );
    return result == ARGON2_OK;
}
