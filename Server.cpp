#include "crow/app.h"
#include "UserDbClass.h"
#include "PasswordDbClass.h"
#include "Server.h"


Server::Server(AppMode mode)
    : mode(mode),
      encryptedLocalStorage("storage.enc", "01234567890123456789012345678901", "1234567890123456"),
      passwordDb("01234567890123456789012345678901", "1234567890123456"),  passwordFilter({})
 {}

void Server::runServer() {
    define_routes();
    app.bindaddr("127.0.0.1").port(18080).run();
}
std::string get_cookie(const crow::request& req, const std::string& cookie_name) {
    auto cookie_header = req.get_header_value("Cookie");
    size_t pos = cookie_header.find(cookie_name + "=");
    if (pos == std::string::npos) return "";

    pos += cookie_name.length() + 1;
    size_t end = cookie_header.find(';', pos);
    if (end == std::string::npos) end = cookie_header.length();

    return cookie_header.substr(pos, end - pos);
}


void Server::define_routes() {
CROW_ROUTE(app, "/").methods("GET"_method)
([this](const crow::request& req) {
    if (mode == AppMode::WEB) {
        std::string token = get_cookie(req, "jwt");

        if (token.empty() || !verify_jwt(token)) {
            std::ifstream loginFile("src/pages/login.html");
            if (!loginFile.is_open()) {
                return crow::response(500, "Cannot open login.html");
            }
            std::stringstream loginBuffer;
            loginBuffer << loginFile.rdbuf();
            return crow::response(401, loginBuffer.str());
        }

        std::ifstream htmlFile("src/pages/index.html");
        if (!htmlFile.is_open()) {
            return crow::response(500, "Cannot open index.html");
        }
        std::stringstream buffer;
        buffer << htmlFile.rdbuf();
        return crow::response(200, buffer.str());
    } else {
        return crow::response("Password Manager Server Running in APP mode");
    }
});


CROW_ROUTE(app, "/css/<string>")
([this](const std::string& filename) {
    std::string filepath = "src/css/" + filename;

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return crow::response(404, "File not found");
    }

    std::ostringstream contents;
    contents << file.rdbuf();

    crow::response res(contents.str());
    res.set_header("Content-Type", "text/css");
    return res;
});


    CROW_ROUTE(app, "/register").methods("GET"_method)
([this]() {
    if (mode == AppMode::WEB) {
        std::ifstream file("src/pages/register.html");
        if (!file.is_open()) return crow::response(404, "register.html not found");
        std::ostringstream content;
        content << file.rdbuf();
        return crow::response(content.str());
    } else {
        return crow::response("Register endpoint (UI mode)");
    }
});

CROW_ROUTE(app, "/login").methods("GET"_method)
([this]() {
    if (mode == AppMode::WEB) {
        std::ifstream file("src/pages/login.html");
        if (!file.is_open()) return crow::response(404, "login.html not found");
        std::ostringstream content;
        content << file.rdbuf();
        return crow::response(content.str());
    } else {
        return crow::response("Login endpoint (UI mode)");
    }
});

CROW_ROUTE(app, "/genPass").methods("GET"_method)
([this]() {
    if (mode == AppMode::WEB) {
        std::ifstream file("src/pages/genPass.html");
        if (!file.is_open()) return crow::response(404, "login.html not found");
        std::ostringstream content;
        content << file.rdbuf();
        return crow::response(content.str());
    } else {
        return crow::response("Login endpoint (UI mode)");
    }
});


CROW_ROUTE(app, "/register").methods("POST"_method)
([this](const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400, "Invalid JSON");

    std::string login = body["login"].s();
    std::string password = body["password"].s();

    if (userDb.create_user(login, password) == 0) {
 std::string token = create_jwt(login);

        crow::response res;
        res.code = 200;

        crow::json::wvalue res_json;
        const int userid = userDb.get_userid(login, password);
        res_json["Userid"] = userid;

        res.body = res_json.dump(); 

        res.add_header("Set-Cookie", "jwt=" + token + "; Path=/; HttpOnly; SameSite=Strict");

        return res;
    } else {
        return crow::response(500, "Registration failed");
    }
});

 CROW_ROUTE(app, "/login").methods("POST"_method)
([this](const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400, "Invalid JSON");

    std::string login = body["login"].s();
    std::string password = body["password"].s();

    if (userDb.login_user(login, password) == 0) {
        std::string token = create_jwt(login);

        crow::response res;
        res.code = 200;

        crow::json::wvalue res_json;
        const int userid = userDb.get_userid(login, password);
        res_json["Userid"] = userid;

        res.body = res_json.dump(); 

        res.add_header("Set-Cookie", "jwt=" + token + "; Path=/; HttpOnly; SameSite=Strict");

        return res;
    } else {
        return crow::response(401, "Invalid credentials");
    }
});


   CROW_ROUTE(app, "/logout").methods("POST"_method)
([this](const crow::request& req) {
    std::string auth = req.get_header_value("Authorization");
 

    crow::response res;
    res.code = 200;

    res.add_header("Set-Cookie", "jwt=; Path=/; HttpOnly; Expires=Thu, 01 Jan 1970 00:00:00 GMT");

    res.body = "Logged out";
    return res;
});

CROW_ROUTE(app, "/add_password").methods("POST"_method)
([this](const crow::request& req) {
    std::string token = get_cookie(req, "jwt");
    if (token.empty() || !verify_jwt(token))
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
        std::string token = get_cookie(req, "jwt");
        if (token.empty() || !verify_jwt(token))
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
        std::string token = get_cookie(req, "jwt");
        if (token.empty() || !verify_jwt(token))
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
    std::string token = get_cookie(req, "jwt");
    if (token.empty() || !verify_jwt(token))
        return crow::response(401, "Unauthorized");

    auto urlParams = crow::query_string(req.url_params);
    if (!urlParams.get("userId"))
        return crow::response(400, "Missing userId");

    int userId = std::stoi(urlParams.get("userId"));
    auto data = passwordDb.get_all_passwords_for_user(userId);

    crow::json::wvalue json;
    for (const auto& [website, pass] : data)
        json[website] = pass;

    return crow::response(json);
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

CROW_ROUTE(app, "/generate_local_storage").methods("GET"_method)
([this](const crow::request&) {
    try {
        std::string data = "temporary_token=abc123;valid_until=2030;";
        encryptedLocalStorage.write(data);
        return crow::response(200, "Encrypted local storage generated.");
    } catch (const std::exception& e) {
        return crow::response(500, std::string("Error: ") + e.what());
    }
});

CROW_ROUTE(app, "/export_passwords_encrypted").methods("GET"_method)
([this](const crow::request& req) {
   std::string token = get_cookie(req, "jwt");
    if (token.empty() || !verify_jwt(token))
        return crow::response(401, "Unauthorized");

    auto urlParams = crow::query_string(req.url_params);
    if (!urlParams.get("userId"))
        return crow::response(400, "Missing userId");

    int user_id = std::stoi(urlParams.get("userId"));
    auto passwords = passwordDb.get_all_passwords_for_user(user_id);

    std::string plain = "{";
    bool first = true;
    for (const auto& pair : passwords) {
        if (!first) plain += ",";
        first = false;
        plain += "\"" + pair.first + "\":\"" + pair.second + "\"";
    }
    plain += "}";

    try {
        std::string encrypted = encryptedLocalStorage.encrypt(plain);
     crow::response resp(encrypted);
    resp.set_header("Content-Type", "application/octet-stream");
    return resp;

    } catch (const std::exception& e) {
        return crow::response(500, std::string("Encryption error: ") + e.what());
    }
});


CROW_ROUTE(app, "/import_passwords_encrypted").methods("POST"_method)
([this](const crow::request& req) {
    auto urlParams = crow::query_string(req.url_params); 
    int user_id = std::stoi(urlParams.get("userId"));

    try {
        std::string decrypted = encryptedLocalStorage.decrypt(req.body);
        auto json = crow::json::load(decrypted);

        if (!json) {
            return crow::response(400, "Invalid decrypted JSON.");
        }

        for (const auto& key : json.keys()) {
            std::string website = key;
            std::string password = json[key].s();
            passwordDb.add_password(website, password, user_id);
        }

        return crow::response(200, "Passwords imported and stored securely.");
    } catch (const std::exception& e) {
        return crow::response(500, std::string("Decryption/import error: ") + e.what());
    }
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
