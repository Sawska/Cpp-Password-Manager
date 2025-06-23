#ifndef PASS_ENCRYPTOR_DECRYPTOR_H
#define PASS_ENCRYPTOR_DECRYPTOR_H

#include <string>

class PassEncryptorDecryptor {
public:
    PassEncryptorDecryptor(const std::string& key, const std::string& iv);

    std::string encrypt(const std::string& plaintext) const;
    std::string decrypt(const std::string& ciphertext) const;

private:
    std::string key_;
    std::string iv_;
};

#endif // PASS_ENCRYPTOR_DECRYPTOR_H
