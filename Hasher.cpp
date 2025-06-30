#include "Hasher.h"
#include <argon2.h>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <vector>
#include "Hasher.h"
#include <argon2.h>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <vector>

std::string Hasher::hash_password(const std::string& password) {
    const uint32_t t_cost = 2;
    const uint32_t m_cost = 1 << 15;
    const uint32_t parallelism = 1;
    const uint32_t saltlen = 8;
    const uint32_t hashlen = 32;

    const char* salt = "somesalt";

    size_t encoded_len = argon2_encodedlen(t_cost, m_cost, parallelism, saltlen, hashlen, Argon2_id);
    std::vector<char> encoded(encoded_len + 1, 0);

    int result = argon2_hash(
        t_cost,
        m_cost,
        parallelism,
        password.c_str(), password.size(),
        salt, saltlen,
        nullptr, hashlen,             
        encoded.data(), encoded.size(),
        Argon2_id,
        ARGON2_VERSION_13
    );

    if (result != ARGON2_OK) {
        std::cerr << "Argon2 error: " << argon2_error_message(result) << std::endl;
        throw std::runtime_error("Argon2 hashing failed");
    }

    encoded[encoded_len] = '\0';

    return std::string(encoded.data());
}


bool Hasher::verify_password(const std::string& password, const std::string& hash) {
    if (hash.empty() || hash.find("$argon2id$") != 0) {
      
        return false;
    }

    return argon2_verify(
        hash.c_str(),                
        password.c_str(), password.size(),
        Argon2_id
    ) == ARGON2_OK;
}
