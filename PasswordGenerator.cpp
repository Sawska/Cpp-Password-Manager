#include "PasswordGenerator.h"
#include <random>
#include <algorithm>



std::string PasswordGenerator::generate_16_character_pass() {
    return generate_password(16);
}

std::string PasswordGenerator::generate_8_character_pass() {
    return generate_password(8);
}

std::string PasswordGenerator::generate_password(std::size_t length) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string password;
    password.reserve(length);

    for (std::size_t i = 0; i < length; ++i) {
        password += chars[dist(gen)];
    }

    return password;
}
