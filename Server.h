#ifndef SERVER_H
#define SERVER_H

#include "crow/app.h"
#include "UserDbClass.h"
#include "PasswordDbClass.h"
#include <jwt-cpp/jwt.h>
#include <unordered_set>
#include "PasswordFilter.h"
#include "PasswordGenerator.h"
#include "EncryptedLocalStorage.h"
#include "Mode.h"

class Server {
  public:
    Server(AppMode mode);           
    void runServer();

  private:
    AppMode mode;
    const std::string jwt_secret = "your-256-bit-secret";
    crow::SimpleApp app;
    UserDb userDb;
    PasswordDb passwordDb;
    PasswordFilter passwordFilter;
    std::unordered_set<std::string> tokenBlacklist;
    PasswordGenerator passwordGenerator;
    EncryptedLocalStorage encryptedLocalStorage;

    void define_routes();
    std::string create_jwt(const std::string& username);
    bool verify_jwt(const std::string& token);
};

#endif // SERVER_H
