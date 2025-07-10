#include <gtest/gtest.h>
#include "../PasswordDbClass.h"


TEST(PasswordDbTest, AddUpdateDeletePassword) {
    PasswordDb db("thisisakey123456", "thisisaniv123456");
    int userId = 1;

    db.add_password("testsite.com", "mypassword123", userId);
    auto all = db.get_all_passwords_for_user(userId);
    ASSERT_TRUE(all.count("testsite.com"));

    db.update_password("newsite.com", "newpass", userId, "testsite.com", "mypassword123");
    all = db.get_all_passwords_for_user(userId);
    ASSERT_TRUE(all.count("newsite.com"));

    db.delete_password("newsite.com", "newpass", userId);
    all = db.get_all_passwords_for_user(userId);
    ASSERT_FALSE(all.count("newsite.com"));
}
