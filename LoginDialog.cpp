#include "LoginDialog.h"

LoginDialog::LoginDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Login/Register", wxDefaultPosition, wxSize(300, 200)) {
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    loginCtrl = new wxTextCtrl(this, wxID_ANY);
    passwordCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    registerCheck = new wxCheckBox(this, wxID_ANY, "Register instead of Login");
    wxButton* submitBtn = new wxButton(this, wxID_OK, "Submit");

     submitBtn->Bind(wxEVT_BUTTON, &LoginDialog::OnSubmit, this);

    vbox->Add(new wxStaticText(this, wxID_ANY, "Login:"), 0, wxALL, 5);
    vbox->Add(loginCtrl, 0, wxALL | wxEXPAND, 5);
    vbox->Add(new wxStaticText(this, wxID_ANY, "Password:"), 0, wxALL, 5);
    vbox->Add(passwordCtrl, 0, wxALL | wxEXPAND, 5);
    vbox->Add(registerCheck, 0, wxALL, 5);
    vbox->Add(submitBtn, 0, wxALL | wxALIGN_CENTER, 5);

    SetSizer(vbox);
    Centre();
}

std::string LoginDialog::GetLogin() const {
    return loginCtrl->GetValue().ToStdString();
}

std::string LoginDialog::GetPassword() const {
    return passwordCtrl->GetValue().ToStdString();
}

bool LoginDialog::IsRegistering() const {
    return registerCheck->IsChecked();
}

std::string LoginDialog::GetJwtToken() const {
    return jwtToken;
}

int LoginDialog::GetUserId() const {
    return userId;
}

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <wx/msgdlg.h> 

void LoginDialog::OnSubmit(wxCommandEvent&) {
    std::string login = GetLogin();
    std::string password = GetPassword();

    if (login.empty() || password.empty()) {
        wxMessageBox("Login and password must not be empty", "Validation Error", wxOK | wxICON_WARNING);
        return;
    }

    httplib::Client client("http://127.0.0.1:18080");
    httplib::Headers headers = {
        {"Content-Type", "application/json"}
    };

    std::string route = IsRegistering() ? "/register" : "/login";
    std::string body = "{\"login\":\"" + login + "\", \"password\":\"" + password + "\"}";

    auto res = client.Post(route.c_str(), headers, body, "application/json");

    if (res && res->status == 200) {
        try {
            auto json = nlohmann::json::parse(res->body);
            userId = json["Userid"];
        } catch (...) {
            wxMessageBox("Failed to parse response from server", "Error", wxOK | wxICON_ERROR);
            return;
        }

        jwtToken = res->get_header_value("Set-Cookie");
        EndModal(wxID_OK);
    } else {
        std::string msg = "Login/Register failed";
        if (res) {
            msg += ": " + res->body;
        } else {
            msg += ": No response from server.";
        }
        wxMessageBox(msg, "Authentication Failed", wxOK | wxICON_ERROR);
    }
}
