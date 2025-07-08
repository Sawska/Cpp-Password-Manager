#include <gtest/gtest.h>
#include "Server.h"
#include "crow/http_request.h"
#include "crow/http_response.h"

class ServerTest : public ::testing::Test {
protected:
    Server server{AppMode::WEB};
};


TEST_F(ServerTest, CreateAndVerifyJwt) {
    std::string token = server.create_jwt("testuser");
    ASSERT_FALSE(token.empty());

    bool valid = server.verify_jwt(token);
    EXPECT_TRUE(valid);
}

TEST_F(ServerTest, JwtIsRejectedIfInvalid) {
    std::string fake_token = "some.invalid.token";
    EXPECT_FALSE(server.verify_jwt(fake_token));
}


TEST(ServerCookieTest, ExtractsCookieCorrectly) {
    crow::request req;
    req.headers.insert({"Cookie", "jwt=mytesttoken; other=value;"});

    std::string value = get_cookie(req, "jwt");
    EXPECT_EQ(value, "mytesttoken");

    value = get_cookie(req, "other");
    EXPECT_EQ(value, "value");

    value = get_cookie(req, "nonexistent");
    EXPECT_EQ(value, "");
}


