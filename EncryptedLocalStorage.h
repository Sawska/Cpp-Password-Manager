#ifndef ENCRYPTED_LOCAL_STORAGE_H
#define ENCRYPTED_LOCAL_STORAGE_H

#include <string>
#include "PassEncryptorDecryptor.h"

class EncryptedLocalStorage {
public:
    EncryptedLocalStorage(const std::string& file_path, const std::string& key, const std::string& iv);
    std::string encrypt(const std::string& plain);
    std::string decrypt(const std::string& encrypted);

    void write(const std::string& data);
    std::string read();

private:
    std::string file_path_;
    std::string key_;
    std::string iv_;
    PassEncryptorDecryptor aes_;
};

#endif // ENCRYPTED_LOCAL_STORAGE_H
