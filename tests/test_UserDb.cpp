#include <gtest/gtest.h>
#include "../UserDbClass.h"

TEST(UserDbTest, CreateLoginDeleteUser) {
    UserDb db;
    db.create_user("testuser", "testpass");

    int login = db.login_user("testuser", "testpass");
    EXPECT_EQ(login, 1);

    int id = db.get_userid("testuser", "testpass");
    EXPECT_GT(id, 0);

    int del = db.delete_user("testuser");
    EXPECT_EQ(del, 1);

    int fail_login = db.login_user("testuser", "testpass");
    EXPECT_EQ(fail_login, 0);
}
