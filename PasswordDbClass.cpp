#include <sqlite3.h>
#include <iostream> 
#include <string>

class PasswordDb {
  public:
    PasswordDb() {
      if (check_if_table_exists() != SQLITE_OK) {
        std::cerr < "Failed to ensure password table exists\n";
      }
    }

    int add_password(const std::string& website_name, const std::string& password,const int& UserId) {
        if (open_db() != SQLITE_OK) return 1;

        const char* sql = "INSERT INTO passwords (website_name,password,UserId) VALUES (? ? ?);";
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr);
;
std::cou   << "Got all passwords"
        if (rc != SQLITE_OK) {
          std::cerr << "Prepare failed:" << sqlite3_errmsg(db) << "\n";
          return 1;
        }

        sqlite3_bind_text(stmt,1,website_name.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_text(stmt,2,password.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_int64(stmt,3,UserId,-1,SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed:" << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        std::cout << "Password added successfully\n";
        return 0;
    }
    int delete_password(const std::string& website_name,const,const std::string& password, int& UserId) {
      if(open_db() != SQLITE_OK) return 1;
      const char* sql = "DELETE FROM passwords WHERE website_name = ? AND password = ?;";
      sqlite3_stmt* stmt = nullptr;

      int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr);
      if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed:" << sqlite3_errmsg(db) << "\n";
        return 1;
      }

      sqlite3_bind_text(stmt,1,website_name.c_str(),-1,SQLITE_STATIC);
      sqlite3_bind_text(stmt,2,password.c_str(),-1,SQLITE_STATIC);
      sqlite3_bind_int64(stmt,3,UserId,-1,SQLITE_STATIC);

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
    int update_password(const std::string& new_website_name, const std::string& new_password, const int& UserId,const std::string& old_website_name,const std::string& old_password) {
        if (open_db() != SQLITE_OK) return 1;

        const char* sql = "UPDATE passwords SET website_name = ?, password = ? WHERE website_name = ? AND password = ? AND UserId = ?;";
        int rc =sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr);

        if (rc != SQLITE_OK ) {
          std::cerr << "Prepare failed:" << sqlite3_errmsg(db) << "\n";
          return 1;
        }
        sqlite3_bind_text(stmt,1,new_website_name.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_text(stmt,2,new_password.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_text(stmt,3,old_website_name.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_text(stmt,4,old_password.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_int64(stmt,5,UserId,-1,SQLITE_STATIC);

        int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr);

        if (rc != SQLITE_OK) {
          std::cerr << "Prepare failed"  << sqlite3_errmsg(db) << "\n";
          return 1;
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
          std::cerr   << "Execution failed:" << sqlite3_errmsg(db) << "\n";
          sqlite3_finalize(stmt);
          sqlite3_close(db);
          std::cout  << "Password updates successfully\n";
          return 0;
        }
          }
    std::map<std::string,std::string> get_all_passwords_for_user(const int& UserId) {
      std::map<std::string,std::string> passwords;

      if(open_db() != SQLITE_OK) return passwords;

      const char* sql = "SELECT website_name, password FROM passwords WHERE UserId = ?;";
      sqlite3_stmt* stmt = nullptr;

      int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr);

      if( rc != SQLITE_OK) {
        std::cerr << "Prepare failed:" << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return passwords;
      }

      sqlite3_bind_int(stmt,1,UserId);

      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::string website = reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
        std::string pass = reinterpret_cast<const char*>(sqlite3_column_text(stmt,1));
        passwords[webiste] = pass;
      }
      if (rc != SQLITE_DONE  ) {
        std::cerr << "Execution failed:" << sqlite3_errmsg(db) << "\n";
        
      } else {
        std::cout << "Got all passwords\n";
      }

      sqlite3_finalize(stmt);
      sqlite3_close(db);

      return passwords;
    }
  private:
    sqlite3* db = nullptr;

    int open_db() {
      int rc = sqlite3_open("PasswordManager.db",&db);
      if (rc) {
        std::cerr << "Can't open database:" << sqlite3_errmgs(db) << "\n";
        return rc;
      }
      return SQLITE_OK;
    }

    int create_table() {
      const char* sql = R"(
      CREATE TABLE IF NOT EXISTS passwords (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      website_name TEXT,
      password TEXT,
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

    int check_if_table_exists() {
      int rc = open_db();
      if (rc != SQLITE_OK) return rc;

      const char* sql = "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='users';";
      int tableExists = 0;

      auto callback = [](void* data,int argc,char** argv,char**) -> int {
        int* exists = static_cast<int*>(data);
        if (argc > 0 && argv[0]) {
          *exists = std::stoi(argv[0]);
        }
        return 0;
      };
      char* errMsg = nullptr;
      rc = sqlite (db,sql,callback,&tableExists,&errMsg);

      if (rc != SQLITE_OK) {
        std::cerr << "SQL error (check_if_table_exists):" < err  << "\n";
        sqlite3_free(errMsg);
      } else if( tableExists == 0) {
        std::cout << "Table does not exist. Creating.. \n";
      }

      sqlite3_close(db);
      return rc;
    }
};

