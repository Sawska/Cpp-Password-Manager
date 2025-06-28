#include "EncryptedLocalStorage.h"
#include <fstream>
#include <vector>
#include <stdexcept>

EncryptedLocalStorage::EncryptedLocalStorage(const std::string& file_path, const std::string& key, const std::string& iv)
    : file_path_(file_path), key_(key), iv_(iv), aes_(key, iv) {}

void EncryptedLocalStorage::write(const std::string& data) {
    std::string encrypted = aes_.encrypt(data);

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

    return aes_.decrypt(std::string(buffer.begin(), buffer.end()));
}

std::string EncryptedLocalStorage::encrypt(const std::string& plain) {
    return aes_.encrypt(plain);
}

std::string EncryptedLocalStorage::decrypt(const std::string& encrypted) {
    return aes_.decrypt(encrypted);
}
