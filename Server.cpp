#include "crow/app.h"
#include "UserDbClass.h"
#include "PasswordDbClass.h"
#include "Server.h"

void Server::runServer() {
    define_routes();
    app.bindaddr("127.0.0.1").port(18080).run();
}


void Server::define_routes() {
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

        if (userDb.login_user(login, password) == 0) {
            crow::json::wvalue res;
            res["token"] = create_jwt(login);
            return crow::response(200, res);
        } else {
            return crow::response(401, "Invalid credentials");
        }
    });

    CROW_ROUTE(app, "/logout").methods("POST"_method)
    ([this](const crow::request& req) {
        std::string auth = req.get_header_value("Authorization");
        if (auth.empty() || auth.find("Bearer ") != 0)
            return crow::response(400, "Missing or invalid Authorization header");

        std::string token = auth.substr(7);
        tokenBlacklist.insert(token);
        return crow::response(200, "Logged out");
    });

    CROW_ROUTE(app, "/add_password").methods("POST"_method)
    ([this](const crow::request& req) {
        std::string auth = req.get_header_value("Authorization");
        if (auth.empty() || auth.find("Bearer ") != 0 || !verify_jwt(auth.substr(7)))
            return crow::response(401, "Unauthorized");

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
        std::string auth = req.get_header_value("Authorization");
        if (auth.empty() || auth.find("Bearer ") != 0 || !verify_jwt(auth.substr(7)))
            return crow::response(401, "Unauthorized");

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
        std::string auth = req.get_header_value("Authorization");
        if (auth.empty() || auth.find("Bearer ") != 0 || !verify_jwt(auth.substr(7)))
            return crow::response(401, "Unauthorized");

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
        std::string auth = req.get_header_value("Authorization");
        if (auth.empty() || auth.find("Bearer ") != 0 || !verify_jwt(auth.substr(7)))
            return crow::response(401, "Unauthorized");

        auto urlParams = crow::query_string(req.url_params);
        if (!urlParams.get("userId")) return crow::response(400, "Missing userId");

        int userId = std::stoi(urlParams.get("userId"));
        auto data = passwordDb.get_all_passwords_for_user(userId);

        crow::json::wvalue json;
        for (const auto& [website, pass] : data)
            json[website] = pass;

        return crow::response(json);
    });

    CROW_ROUTE(app, "/").methods("GET"_method)
    ([] {
        return crow::response("Password Manager Server Running");
    });

CROW_ROUTE(app, "/filter_passwords_by_website/<string>").methods("GET"_method)
([this](const crow::request& req, std::string website_name) {
    auto urlParams = crow::query_string(req.url_params);
    if (!urlParams.get("userId")) {
        return crow::response(400, "Missing userId");
    }

    int userId = std::stoi(urlParams.get("userId"));

    
    auto all_passwords = passwordDb.get_all_passwords_for_user(userId);

    
    PasswordFilter filter(all_passwords);

    
    auto filtered = filter.filter_by_website(website_name);

    crow::json::wvalue res_json;
    for (const auto& [website, pass] : filtered) {
        res_json[website] = pass;
    }

    return crow::response(res_json);
});

CROW_ROUTE(app,"/generate_password/<int>").methods("GET"_method)
([this](const crow::request& req, int length) {
    std::string pass;
    if(length == 16) {
        pass = passwordGenerator.generate_16_character_pass();
    } else if(length == 8) {
        pass = passwordGenerator.generate_8_character_pass();
    } else {
        crow::json::wvalue error_json;
        error_json["error"] = "Invalid size of generated password, choose between 16 or 8";
        return crow::response(400, error_json);
    }

    crow::json::wvalue json;
    json["generated_password"] = pass;
    return crow::response(json);
});


}


std::string Server::create_jwt(const std::string& username) {
    return jwt::create()
        .set_issuer("yourapp")
        .set_type("JWS")
        .set_payload_claim("username", jwt::claim(username))
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes{60})
        .sign(jwt::algorithm::hs256{jwt_secret});
}

bool Server::verify_jwt(const std::string& token) {
    try {
        if (tokenBlacklist.count(token)) return false;
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{jwt_secret})
            .with_issuer("yourapp");
        verifier.verify(decoded);
        return true;
    } catch (...) {
        return false;
    }
}