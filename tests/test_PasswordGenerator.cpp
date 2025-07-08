#include <gtest/gtest.h>
#include "PasswordGenerator.h"

TEST(PasswordGeneratorTest, GeneratePasswords) {
    PasswordGenerator gen;
    std::string p16 = gen.generate_16_character_pass();
    std::string p8 = gen.generate_8_character_pass();
    EXPECT_EQ(p16.size(), 16);
    EXPECT_EQ(p8.size(), 8);
}
