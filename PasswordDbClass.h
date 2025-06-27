#ifndef PASSWORD_DB_CLASS

#define PASSWORD_DB_CLASS

#include <sqlite3.h>
#include <string>
#include <map>
#include "PassEncryptorDecryptor.h"

class PasswordDb {
  public:
    PasswordDb(const std::string& key, const std::string& iv);
    int add_password(const std::string& website_name, const std::string& password, const int& UserId);
    int delete_password(const std::string& website_name, const std::string& password, int& UserId);
    int update_password(const std::string& new_website, const std::string& new_password,const int& UserId, const std::string& old_website_name, const std::string& old_password);
    std::map<std::string,std::string> get_all_passwords_for_user(const int& UserId);
  private:
    sqlite3* db = nullptr;
    PassEncryptorDecryptor passEncryptorDecryptor;

    int open_db();
    int create_table();
    int check_if_table_exists();
};

#endif // !PASSWORD_DB_CLASS
