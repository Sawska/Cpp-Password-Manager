#ifndef PASSWORD_GENERATOR_H
#define PASSWORD_GENERATOR_H

#include <string>

class PasswordGenerator {
public:
    std::string generate_16_character_pass();
    std::string generate_8_character_pass();

private:
    std::string generate_password(std::size_t length);
    std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()-_=+";
};

#endif // PASSWORD_GENERATOR_H
