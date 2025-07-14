#include "LoginDialog.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <wx/msgdlg.h>
#include <wx/statbox.h>

static wxStaticText* MakeLabel(wxWindow* parent,
                               const wxString& text,
                               const wxFont&  font,
                               const wxColour& colour)
{
    wxStaticText* lbl = new wxStaticText(parent, wxID_ANY, text);
    lbl->SetFont(font);
    lbl->SetForegroundColour(colour);
    return lbl;
}

LoginDialog::LoginDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "🔐  Account",
               wxDefaultPosition, wxSize(1920, 1080),
               wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN)
{
    const wxColour bgPanel     ("#f5f5f5");
    const wxColour bgInput     ("#ffffff");
    const wxColour fgPrimary   ("#333333");
    const wxColour accent      ("#1976D2");
    const wxColour accentHover ("#1E88E5");
    const wxColour inputBorder ("#cccccc");
    const wxColour focusBorder ("#64B5F6");

    wxFont titleFont (13, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    wxFont labelFont (11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_MEDIUM);
    wxFont inputFont (10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont buttonFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

    SetBackgroundColour(bgPanel);

    wxBoxSizer* root = new wxBoxSizer(wxVERTICAL);

    wxStaticBox* box = new wxStaticBox(this, wxID_ANY, "Credentials");
    box->SetFont(titleFont);

    wxStaticBoxSizer* boxSizer = new wxStaticBoxSizer(box, wxVERTICAL);

    loginCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition,
                               wxSize(-1, 32), wxBORDER_SIMPLE);
    loginCtrl->SetFont(inputFont);
    loginCtrl->SetBackgroundColour(bgInput);
    loginCtrl->SetForegroundColour(fgPrimary);
    loginCtrl->SetOwnForegroundColour(fgPrimary);
    loginCtrl->SetOwnBackgroundColour(bgInput);
    loginCtrl->SetOwnFont(inputFont);

    // Focus border color change
    loginCtrl->Bind(wxEVT_SET_FOCUS, [=](wxFocusEvent&) {
        loginCtrl->SetBackgroundColour("#ffffff");
        loginCtrl->SetWindowStyleFlag(wxBORDER_THEME);
        loginCtrl->Refresh();
    });
    loginCtrl->Bind(wxEVT_KILL_FOCUS, [=](wxFocusEvent&) {
        loginCtrl->SetBackgroundColour(bgInput);
        loginCtrl->Refresh();
    });

    passwordCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition,
                                  wxSize(-1, 32), wxTE_PASSWORD | wxBORDER_SIMPLE);
    passwordCtrl->SetFont(inputFont);
    passwordCtrl->SetBackgroundColour(bgInput);
    passwordCtrl->SetForegroundColour(fgPrimary);

    passwordCtrl->Bind(wxEVT_SET_FOCUS, [=](wxFocusEvent&) {
        passwordCtrl->SetBackgroundColour("#ffffff");
        passwordCtrl->SetWindowStyleFlag(wxBORDER_THEME);
        passwordCtrl->Refresh();
    });
    passwordCtrl->Bind(wxEVT_KILL_FOCUS, [=](wxFocusEvent&) {
        passwordCtrl->SetBackgroundColour(bgInput);
        passwordCtrl->Refresh();
    });

    registerCheck = new wxCheckBox(this, wxID_ANY, "🆕 Register instead of Login");
    registerCheck->SetFont(labelFont);
    registerCheck->SetForegroundColour(fgPrimary);
    registerCheck->SetBackgroundColour(bgPanel);

    wxButton* submitBtn = new wxButton(this, wxID_OK, "✔️  Submit");
    submitBtn->SetFont(buttonFont);
    submitBtn->SetForegroundColour("#FFFFFF");
    submitBtn->SetBackgroundColour(accent);
    submitBtn->SetWindowStyle(wxBORDER_SIMPLE);
    submitBtn->SetMinSize(wxSize(-1, 36));

    // hover effect
    submitBtn->Bind(wxEVT_ENTER_WINDOW, [=](wxMouseEvent&) {
        submitBtn->SetBackgroundColour(accentHover);
        submitBtn->Refresh();
    });
    submitBtn->Bind(wxEVT_LEAVE_WINDOW, [=](wxMouseEvent&) {
        submitBtn->SetBackgroundColour(accent);
        submitBtn->Refresh();
    });

    submitBtn->Bind(wxEVT_BUTTON, &LoginDialog::OnSubmit, this);

    boxSizer->AddSpacer(6);
    boxSizer->Add(MakeLabel(this, "👤 Login:", labelFont, fgPrimary), 0, wxLEFT | wxRIGHT, 8);
    boxSizer->Add(loginCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    boxSizer->Add(MakeLabel(this, "🔑 Password:", labelFont, fgPrimary), 0, wxLEFT | wxRIGHT, 8);
    boxSizer->Add(passwordCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    boxSizer->Add(registerCheck, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);
    boxSizer->Add(submitBtn, 0, wxALIGN_CENTER | wxALL, 8);

    root->Add(boxSizer, 1, wxEXPAND | wxALL, 15);
    SetSizer(root);
    SetMinSize(wxSize(400, 320));
    Centre();
}



std::string LoginDialog::GetLogin()       const { return loginCtrl->GetValue().ToStdString(); }
std::string LoginDialog::GetPassword()    const { return passwordCtrl->GetValue().ToStdString(); }
bool        LoginDialog::IsRegistering()  const { return registerCheck->IsChecked(); }
std::string LoginDialog::GetJwtToken()    const { return jwtToken; }
int         LoginDialog::GetUserId()      const { return userId; }


void LoginDialog::OnSubmit(wxCommandEvent&)
{
    std::string login    = GetLogin();
    std::string password = GetPassword();

    if (login.empty() || password.empty())
    {
        wxMessageBox("Login and password must not be empty",
                     "Validation Error",
                     wxOK | wxICON_WARNING);
        return;
    }

    httplib::Client client("http://127.0.0.1:18080");
    httplib::Headers headers = { { "Content-Type", "application/json" } };

    std::string route = IsRegistering() ? "/register" : "/login";
    std::string body =
        "{\"login\":\"" + login + "\", \"password\":\"" + password + "\"}";

    auto res = client.Post(route.c_str(), headers, body, "application/json");

    if (res && res->status == 200)
    {
        try
        {
            auto json = nlohmann::json::parse(res->body);
            userId   = json["Userid"];
        }
        catch (...)
        {
            wxMessageBox("Failed to parse response from server",
                         "Error", wxOK | wxICON_ERROR);
            return;
        }

        jwtToken = res->get_header_value("Set-Cookie");
        EndModal(wxID_OK);
    }
    else
    {
        std::string msg = "Login/Register failed";
        if (res) msg += ": " + res->body;
        else     msg += ": No response from server.";

        wxMessageBox(msg, "Authentication Failed",
                     wxOK | wxICON_ERROR);
    }
}
