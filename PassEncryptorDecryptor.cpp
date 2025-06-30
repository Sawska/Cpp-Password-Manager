#include "PassEncryptorDecryptor.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <vector>
#include <stdexcept>

PassEncryptorDecryptor::PassEncryptorDecryptor(const std::string& key, const std::string& iv)
    : key_(key), iv_(iv) {
    if (key_.size() != 32) 
        throw std::invalid_argument("Key must be 32 bytes (256 bits)");
    if (iv_.size() != 16) 
        throw std::invalid_argument("IV must be 16 bytes (128 bits)");
}

std::string PassEncryptorDecryptor::encrypt(const std::string& plaintext) const {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create context");

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                                reinterpret_cast<const unsigned char*>(key_.data()),
                                reinterpret_cast<const unsigned char*>(iv_.data())))
        throw std::runtime_error("EncryptInit failed");

    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len, ciphertext_len;

    if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                               reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size()))
        throw std::runtime_error("EncryptUpdate failed");
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len))
        throw std::runtime_error("EncryptFinal failed");
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(ciphertext.data()), ciphertext_len);
}

std::string PassEncryptorDecryptor::decrypt(const std::string& ciphertext) const {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create context");

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                                reinterpret_cast<const unsigned char*>(key_.data()),
                                reinterpret_cast<const unsigned char*>(iv_.data())))
        throw std::runtime_error("DecryptInit failed");

    std::vector<unsigned char> plaintext(ciphertext.size());
    int len, plaintext_len;

    if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                               reinterpret_cast<const unsigned char*>(ciphertext.data()), ciphertext.size()))
        throw std::runtime_error("DecryptUpdate failed");
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len))
        throw std::runtime_error("DecryptFinal failed");
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext_len);
}
