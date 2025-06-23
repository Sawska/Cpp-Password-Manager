#include <sqlite3.h>
#include <iostream> 
#include <string>
#include "PasswordDbClass.h"


PasswordDb::PasswordDb(const std::string& key, const std::string& iv)
    : passEncryptorDecryptor(key, iv) {
    if (check_if_table_exists() != SQLITE_OK) {
        std::cerr << "Failed to ensure password table exists\n";
    }
}


int PasswordDb::add_password(const std::string& website_name, const std::string& password, const int& UserId) {
    if (open_db() != SQLITE_OK) return 1;

    const char* sql = "INSERT INTO passwords (website_name, password, UserId) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    std::cout << "Preparing to add password...\n";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return 1;
    }

    sqlite3_bind_text(stmt, 1, website_name.c_str(), -1, SQLITE_STATIC);
    std::string encryptedPassword = passEncryptorDecryptor.encrypt(password);
    sqlite3_bind_text(stmt, 2, encryptedPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 3, UserId);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    std::cout << "Password added successfully\n";
    return 0;
}

int PasswordDb::delete_password(const std::string& website_name, const std::string& password, int& UserId) {
    if (open_db() != SQLITE_OK) return 1;

    const char* sql = "DELETE FROM passwords WHERE website_name = ? AND password = ? AND UserId = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return 1;
    }

    
    std::string encryptedPassword = passEncryptorDecryptor.encrypt(password);

    sqlite3_bind_text(stmt, 1, website_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, encryptedPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 3, UserId);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    std::cout << "Password deleted successfully\n";
    return 0;
}


int PasswordDb::update_password(const std::string& new_website_name, const std::string& new_password,
                                const int& UserId, const std::string& old_website_name,
                                const std::string& old_password) {
    if (open_db() != SQLITE_OK) return 1;

    const char* sql = "UPDATE passwords SET website_name = ?, password = ? "
                      "WHERE website_name = ? AND password = ? AND UserId = ?;";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return 1;
    }

    
    std::string encryptedNewPassword = passEncryptorDecryptor.encrypt(new_password);
    std::string encryptedOldPassword = passEncryptorDecryptor.encrypt(old_password);

    sqlite3_bind_text(stmt, 1, new_website_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, encryptedNewPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, old_website_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, encryptedOldPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 5, UserId);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    std::cout << "Password updated successfully\n";
    return 0;
}



std::map<std::string,std::string> PasswordDb::get_all_passwords_for_user(const int& UserId) {
    std::map<std::string,std::string> passwords;

    if (open_db() != SQLITE_OK) return passwords;

    const char* sql = "SELECT website_name, password FROM passwords WHERE UserId = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return passwords;
    }

    sqlite3_bind_int(stmt, 1, UserId);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* website_text = sqlite3_column_text(stmt, 0);
        const unsigned char* pass_text = sqlite3_column_text(stmt, 1);

        std::string website = website_text ? reinterpret_cast<const char*>(website_text) : "";
        std::string pass = pass_text ? reinterpret_cast<const char*>(pass_text) : "";

        std::string decryptedPass = passEncryptorDecryptor.decrypt(pass);
        passwords[website] = decryptedPass;

    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << "\n";
    } else {
        std::cout << "Got all passwords\n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (passwords.empty()) {
        std::cout << "No passwords found for user " << UserId << "\n";
    }

    return passwords;
}

int PasswordDb::open_db() {
      int rc = sqlite3_open("PasswordManager.db",&db);
      if (rc) {
        std::cerr << "Can't open database:" << sqlite3_errmsg(db) << "\n";
        return rc;
      }
      return SQLITE_OK;
}

int PasswordDb::create_table() {
const char* sql = R"(
CREATE TABLE IF NOT EXISTS passwords (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  website_name TEXT,
  password TEXT,
  UserId INTEGER,
  FOREIGN KEY (UserId) REFERENCES users(id)
);
)";


      char* errMsg = nullptr;
      int rc = sqlite3_exec(db,sql,nullptr,nullptr,&errMsg);

      if (rc != SQLITE_OK) {
        std::cerr << "SQL error (create_table):" << errMsg << "\n";
        sqlite3_free(errMsg);
      } else {
        std::cout << "Table created successfully\n";
      }
      return rc;
}

int PasswordDb::check_if_table_exists() {
    int rc = open_db();
    if (rc != SQLITE_OK) return rc;

    const char* sql = "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='passwords';";
    int tableExists = 0;

    auto callback = [](void* data, int argc, char** argv, char**) -> int {
        int* exists = static_cast<int*>(data);
        if (argc > 0 && argv[0]) {
            *exists = std::strtol(argv[0], nullptr, 10);
        }
        return 0;
    };

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sql, callback, &tableExists, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error (check_if_table_exists): " << errMsg << "\n";
        sqlite3_free(errMsg);
    } else if (tableExists == 0) {
        std::cout << "Table does not exist. Creating...\n";
        rc = create_table();
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to create table\n";
        }
    } else {
        std::cout << "Table already exists.\n";
    }

    sqlite3_close(db);
    return rc;
}


