#include "EncryptedLocalStorage.h"
#include <fstream>
#include <vector>
#include <stdexcept>

EncryptedLocalStorage::EncryptedLocalStorage(const std::string& file_path, const std::string& key, const std::string& iv)
    : file_path_(file_path), key_(key), iv_(iv) {}

void EncryptedLocalStorage::write(const std::string& data) {
    std::string encrypted = aes_.encrypt(data, key_, iv_);

    std::ofstream ofs(file_path_, std::ios::binary);
    if (!ofs) throw std::runtime_error("Failed to open file for writing");

    ofs.write(encrypted.data(), encrypted.size());
    ofs.close();
}

std::string EncryptedLocalStorage::read() {
    std::ifstream ifs(file_path_, std::ios::binary);
    if (!ifs) throw std::runtime_error("Failed to open file for reading");

    std::vector<char> buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    return aes_.decrypt(std::string(buffer.begin(), buffer.end()), key_, iv_);
}

std::string EncryptedLocalStorage::encrypt(const std::string& plain) {
    return aes_encrypt(plain, key, iv);
}

std::string EncryptedLocalStorage::decrypt(const std::string& encrypted) {
    return aes_decrypt(encrypted, key, iv);
}
