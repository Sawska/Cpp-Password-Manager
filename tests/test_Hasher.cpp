#include <gtest/gtest.h>
#include "../Hasher.h"

TEST(HasherTest, HashAndVerify) {
    Hasher hasher;
    std::string password = "StrongPass123!";
    std::string hash = hasher.hash_password(password);
    ASSERT_TRUE(hasher.verify_password(password, hash));
    ASSERT_FALSE(hasher.verify_password("WrongPass", hash));
}
