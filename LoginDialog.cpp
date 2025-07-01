#include "LoginDialog.h"

LoginDialog::LoginDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Login/Register", wxDefaultPosition, wxSize(300, 200)) {
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    loginCtrl = new wxTextCtrl(this, wxID_ANY);
    passwordCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    registerCheck = new wxCheckBox(this, wxID_ANY, "Register instead of Login");
    wxButton* submitBtn = new wxButton(this, wxID_OK, "Submit");

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
