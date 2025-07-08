#include <gtest/gtest.h>
#include "EncryptedLocalStorage.h"
#include <fstream>

TEST(EncryptedStorageTest, WriteAndReadEncryptedFile) {
    std::string file = "test_secure.dat";
    std::string key = "thisisakey123456";
    std::string iv = "thisisaniv123456";

    EncryptedLocalStorage storage(file, key, iv);
    std::string original = "my super secret data";
    storage.write(original);
    std::string result = storage.read();
    EXPECT_EQ(original, result);

    std::remove(file.c_str());
}
