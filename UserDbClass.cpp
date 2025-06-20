#include <sqlite3.h>
#include <iostream>
#include <string>

class UserDb {
public:
    UserDb() {
        if (check_if_table_exists() != SQLITE_OK) {
            std::cerr << "Failed to ensure users table exists\n";
        }
    }

    int create_user(const std::string& username, const std::string& password) {
        if (open_db() != SQLITE_OK) return 1;

        const char* sql = "INSERT INTO users (login, password) VALUES (?, ?);";
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
            return 1;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Execution failed: " << sqlite3_errmsg(db) << "\n";
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 1;
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        std::cout << "User created successfully\n";
        return 0;
    }

    int delete_user(const std::string& username) {
        if (open_db() != SQLITE_OK) return 1;

        const char* sql = "DELETE FROM users WHERE login = ?;";
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
            return 1;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Execution failed: " << sqlite3_errmsg(db) << "\n";
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 1;
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        std::cout << "User deleted successfully\n";
        return 0;
    }

    int update_user(const std::string& new_username, const std::string& new_password, const std::string& old_username) {
        if (open_db() != SQLITE_OK) return 1;

        const char* sql = "UPDATE users SET login = ?, password = ? WHERE login = ?;";
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
            return 1;
        }

        sqlite3_bind_text(stmt, 1, new_username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, new_password.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, old_username.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Execution failed: " << sqlite3_errmsg(db) << "\n";
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 1;
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        std::cout << "User updated successfully\n";
        return 0;
    }

    int login_user(const std::string& username, const std::string& password) {
        if (open_db() != SQLITE_OK) return 1;

        const char* sql = "SELECT COUNT(*) FROM users WHERE login = ? AND password = ?;";
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
            return 1;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW) {
            std::cerr << "Execution failed: " << sqlite3_errmsg(db) << "\n";
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 1;
        }

        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        if (count == 1) {
            std::cout << "Login successful\n";
            return 0;
        } else {
            std::cout << "Invalid credentials\n";
            return 1;
        }
    }

private:
    sqlite3* db = nullptr;

    int open_db() {
        int rc = sqlite3_open("passwordManager.db", &db);
        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << "\n";
            return rc;
        }
        return SQLITE_OK;
    }

    int create_table() {
        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                login TEXT NOT NULL UNIQUE,
                password TEXT NOT NULL
            );
        )";

        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error (create_table): " << errMsg << "\n";
            sqlite3_free(errMsg);
        } else {
            std::cout << "Table created successfully\n";
        }

        return rc;
    }

    int check_if_table_exists() {
        int rc = open_db();
        if (rc != SQLITE_OK) return rc;

        const char* sql = "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='users';";
        int tableExists = 0;

        auto callback = [](void* data, int argc, char** argv, char**) -> int {
            int* exists = static_cast<int*>(data);
            if (argc > 0 && argv[0]) {
                *exists = std::stoi(argv[0]);
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
        } else {
            std::cout << "Table already exists.\n";
        }

        sqlite3_close(db);
        return rc;
    }
};

