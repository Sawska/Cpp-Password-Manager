#ifndef USER_DB_CLASS_H

#define USER_DB_CLASS_H

#include <sqlite3.h>
#include <string>
#include "Hasher.h"

 class UserDb {
  public:
    UserDb();
    int create_user(const std::string& username,const std::string& password);
    int delete_user(const std::string& username);
    int update_user(const std::string& new_username, const std::string& new_password, const std::string& old_username);
    int login_user(const std::string& username, const std::string& password);
    int get_userid(const std::string& username, const std::string& password);
  private:
    sqlite3* db = nullptr;
    Hasher hasher;
    int open_db();
    int create_table();
    int check_if_table_exists();

};

#endif // !USER_DB_CLASS_H


