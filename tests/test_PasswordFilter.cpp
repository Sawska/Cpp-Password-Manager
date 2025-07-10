#include <gtest/gtest.h>
#include "../PasswordFilter.h"

TEST(PasswordFilterTest, FilterByWebsite) {
    std::map<std::string, std::string> db = {
        {"gmail.com", "pass1"},
        {"yahoo.com", "pass2"},
        {"github.com", "pass3"}
    };
    PasswordFilter filter(db);
    auto result = filter.filter_by_website("git");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result["github.com"], "pass3");
}
