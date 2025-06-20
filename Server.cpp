#include "crow.h"
#include "UserDb.h"
#include "PasswordDb.h"

class Server {
public:
    void runServer() {
        define_routes();
        app.port(18080).run();
    }

private:
    crow::SimpleApp app;
    UserDb userDb;
    PasswordDb passwordDb;

    void define_routes() {
        CROW_ROUTE(app, "/register").methods("POST"_method)
        ([this](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) return crow::response(400, "Invalid JSON");

            std::string login = body["login"].s();
            std::string password = body["password"].s();

            if (userDb.create_user(login, password) == 0)
                return crow::response(200, "User registered");
            else
                return crow::response(500, "Registration failed");
        });

        CROW_ROUTE(app, "/login").methods("POST"_method)
        ([this](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) return crow::response(400, "Invalid JSON");

            std::string login = body["login"].s();
            std::string password = body["password"].s();

            if (userDb.login_user(login, password) == 0)
                return crow::response(200, "Login successful");
            else
                return crow::response(401, "Invalid credentials");
        });

        CROW_ROUTE(app, "/add_password").methods("POST"_method)
        ([this](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) return crow::response(400, "Invalid JSON");

            std::string website = body["website"].s();
            std::string password = body["password"].s();
            int userId = body["userId"].i();

            if (passwordDb.add_password(website, password, userId) == 0)
                return crow::response(200, "Password added");
            else
                return crow::response(500, "Failed to add password");
        });

        CROW_ROUTE(app, "/delete_password").methods("POST"_method)
        ([this](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) return crow::response(400, "Invalid JSON");

            std::string website = body["website"].s();
            std::string password = body["password"].s();
            int userId = body["userId"].i();

            if (passwordDb.delete_password(website, password, userId) == 0)
                return crow::response(200, "Password deleted");
            else
                return crow::response(500, "Failed to delete password");
        });

        CROW_ROUTE(app, "/update_password").methods("POST"_method)
        ([this](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) return crow::response(400, "Invalid JSON");

            std::string newWebsite = body["new_website"].s();
            std::string newPassword = body["new_password"].s();
            std::string oldWebsite = body["old_website"].s();
            std::string oldPassword = body["old_password"].s();
            int userId = body["userId"].i();

            if (passwordDb.update_password(newWebsite, newPassword, userId, oldWebsite, oldPassword) == 0)
                return crow::response(200, "Password updated");
            else
                return crow::response(500, "Failed to update password");
        });

        CROW_ROUTE(app, "/get_passwords").methods("GET"_method)
        ([this](const crow::request& req) {
            auto urlParams = crow::query_string(req.url_params);
            if (!urlParams.get("userId")) return crow::response(400, "Missing userId");

            int userId = std::stoi(urlParams.get("userId"));
            auto data = passwordDb.get_all_passwords_for_user(userId);

            crow::json::wvalue json;
            for (const auto& [website, pass] : data) {
                json[website] = pass;
            }
            return crow::response(json);
        });

        // Optional: for testing
        CROW_ROUTE(app, "/").methods("GET"_method)
        ([] {
            return crow::response("Password Manager Server Running");
        });
    }
};

