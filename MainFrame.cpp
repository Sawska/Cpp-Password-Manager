#include "MainFrame.h"
#include "LoginDialog.h"
#include <httplib.h>
#include <nlohmann/json.hpp>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(1001, MainFrame::OnRefresh)
    EVT_BUTTON(1002, MainFrame::OnAdd)
    EVT_BUTTON(1003, MainFrame::OnDelete)
    EVT_BUTTON(1004, MainFrame::OnUpdate)
    EVT_BUTTON(1005, MainFrame::OnGenerate)
    EVT_BUTTON(1006, MainFrame::OnLogout)
    EVT_BUTTON(1007, MainFrame::OnFilter)
    EVT_BUTTON(1008, MainFrame::OnExport)
    EVT_BUTTON(1009, MainFrame::OnImport)
    EVT_BUTTON(1010, MainFrame::OnGenerate8)

wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const std::string& jwt, int userId)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(600, 400)),
      jwt_token(jwt),
      user_id(userId) {

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

   
    LoginDialog dlg(this);
    if (dlg.ShowModal() != wxID_OK) {
        Close();
        return;
    }

    std::string login = dlg.GetLogin();
    std::string password = dlg.GetPassword();
    bool isRegister = dlg.IsRegistering();

    httplib::Client client("http://127.0.0.1:18080");
    httplib::Headers headers = {{"Content-Type", "application/json"}};
    std::string body = "{\"login\":\"" + login + "\",\"password\":\"" + password + "\"}";
    std::string endpoint = isRegister ? "/register" : "/login";

    auto res = client.Post(endpoint.c_str(), headers, body, "application/json");

    if (!res || res->status != 200) {
        wxMessageBox("Login/Register failed", "Error", wxICON_ERROR);
        Close();
        return;
    }

    jwt_token = res->get_header_value("Set-Cookie");
    auto json = nlohmann::json::parse(res->body);
    user_id = json["Userid"];


    websiteInput = new wxTextCtrl(panel, wxID_ANY);
    passwordInput = new wxTextCtrl(panel, wxID_ANY);
    wxStaticText* labelWebsite = new wxStaticText(panel, wxID_ANY, "Website (Add):");
    wxStaticText* labelPassword = new wxStaticText(panel, wxID_ANY, "Password (Add):");
    wxButton* addButton = new wxButton(panel, 1002, "Add Password");

    deleteButton = new wxButton(panel, 1003, "Delete Selected Password");

    oldWebsiteInput = new wxTextCtrl(panel, wxID_ANY);
    oldPasswordInput = new wxTextCtrl(panel, wxID_ANY);
    newWebsiteInput = new wxTextCtrl(panel, wxID_ANY);
    newPasswordInput = new wxTextCtrl(panel, wxID_ANY);
    wxStaticText* labelOldWebsite = new wxStaticText(panel, wxID_ANY, "Old Website:");
    wxStaticText* labelOldPassword = new wxStaticText(panel, wxID_ANY, "Old Password:");
    wxStaticText* labelNewWebsite = new wxStaticText(panel, wxID_ANY, "New Website:");
    wxStaticText* labelNewPassword = new wxStaticText(panel, wxID_ANY, "New Password:");
    wxButton* updateButton = new wxButton(panel, 1004, "Update Password");
    wxButton* generate8Btn = new wxButton(panel, 1010, "Generate 8-char Password");
    wxButton* logoutBtn = new wxButton(panel, 1006, "Logout");
    wxButton* filterBtn = new wxButton(panel, 1007, "Filter by Website");
    wxButton* exportBtn = new wxButton(panel, 1008, "Export Passwords");
    wxButton* importBtn = new wxButton(panel, 1009, "Import Passwords");

    vbox->Add(generate8Btn, 0, wxALL | wxALIGN_CENTER, 5);
    vbox->Add(filterBtn, 0, wxALL | wxALIGN_CENTER, 5);
    vbox->Add(exportBtn, 0, wxALL | wxALIGN_CENTER, 5);
    vbox->Add(importBtn, 0, wxALL | wxALIGN_CENTER, 5);
    vbox->Add(logoutBtn, 0, wxALL | wxALIGN_CENTER, 5);


    passwordList = new wxListBox(panel, wxID_ANY);
    wxButton* refreshBtn = new wxButton(panel, 1001, "Refresh Passwords");

    wxButton* generateButton = new wxButton(panel, 1005, "Generate 16-char Password");

    statusBox = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE | wxTE_READONLY);


    vbox->Add(refreshBtn, 0, wxALL, 5);

    vbox->Add(labelWebsite, 0, wxLEFT | wxRIGHT, 5);
    vbox->Add(websiteInput, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
    vbox->Add(labelPassword, 0, wxLEFT | wxRIGHT, 5);
    vbox->Add(passwordInput, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
    vbox->Add(addButton, 0, wxALL | wxALIGN_CENTER, 5);
    vbox->Add(deleteButton, 0, wxALL | wxALIGN_CENTER, 5);

    vbox->Add(labelOldWebsite, 0, wxLEFT | wxRIGHT, 5);
    vbox->Add(oldWebsiteInput, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
    vbox->Add(labelOldPassword, 0, wxLEFT | wxRIGHT, 5);
    vbox->Add(oldPasswordInput, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
    vbox->Add(labelNewWebsite, 0, wxLEFT | wxRIGHT, 5);
    vbox->Add(newWebsiteInput, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
    vbox->Add(labelNewPassword, 0, wxLEFT | wxRIGHT, 5);
    vbox->Add(newPasswordInput, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
    vbox->Add(updateButton, 0, wxALL | wxALIGN_CENTER, 5);

    vbox->Add(generateButton, 0, wxALL | wxALIGN_CENTER, 5);

    vbox->Add(passwordList, 1, wxALL | wxEXPAND, 5);
    vbox->Add(statusBox, 0, wxALL | wxEXPAND, 5);

    panel->SetSizer(vbox);

    FetchPasswords();
}

void MainFrame::FetchPasswords() {
    httplib::Client client("http://127.0.0.1:18080");
    std::string url = "/get_passwords?userId=" + std::to_string(user_id);
    httplib::Headers headers = {{"Cookie", jwt_token}};
    auto res = client.Get(url.c_str(), headers);

    passwordList->Clear();

    if (!res || res->status != 200) {
        statusBox->SetValue("❌ Failed to fetch passwords.");
        return;
    }

    auto json = nlohmann::json::parse(res->body);
    for (auto& [website, password] : json.items()) {
        passwordList->Append(website + ": " + password.get<std::string>());
    }

    statusBox->SetValue("✅ Passwords fetched.");
}

void MainFrame::OnRefresh(wxCommandEvent&) {
    FetchPasswords();
}

void MainFrame::OnAdd(wxCommandEvent&) {
    std::string website = websiteInput->GetValue().ToStdString();
    std::string password = passwordInput->GetValue().ToStdString();

    if (website.empty() || password.empty()) {
        statusBox->SetValue("Please enter website and password.");
        return;
    }

    httplib::Client client("http://127.0.0.1:18080");
    httplib::Headers headers = {{"Content-Type", "application/json"}, {"Cookie", jwt_token}};
    std::string body = "{\"website\":\"" + website + "\",\"password\":\"" + password + "\",\"userId\":" + std::to_string(user_id) + "}";

    auto res = client.Post("/add_password", headers, body, "application/json");

    if (res && res->status == 200) {
        statusBox->SetValue("✅ Password added.");
        FetchPasswords();
    } else {
        statusBox->SetValue("❌ Failed to add password.");
    }
}

void MainFrame::OnDelete(wxCommandEvent&) {
    int sel = passwordList->GetSelection();
    if (sel == wxNOT_FOUND) {
        statusBox->SetValue("Select a password to delete.");
        return;
    }

    std::string selected = passwordList->GetString(sel).ToStdString();
    size_t colon = selected.find(':');
    if (colon == std::string::npos) {
        statusBox->SetValue("Invalid format.");
        return;
    }

    std::string website = selected.substr(0, colon);
    std::string password = selected.substr(colon + 2);

    httplib::Client client("http://127.0.0.1:18080");
    httplib::Headers headers = {{"Content-Type", "application/json"}, {"Cookie", jwt_token}};
    std::string body = "{\"website\":\"" + website + "\",\"password\":\"" + password + "\",\"userId\":" + std::to_string(user_id) + "}";

    auto res = client.Post("/delete_password", headers, body, "application/json");

    if (res && res->status == 200) {
        statusBox->SetValue("✅ Password deleted.");
        FetchPasswords();
    } else {
        statusBox->SetValue("❌ Failed to delete password.");
    }
}

void MainFrame::OnUpdate(wxCommandEvent&) {
    std::string oldWebsite = oldWebsiteInput->GetValue().ToStdString();
    std::string oldPassword = oldPasswordInput->GetValue().ToStdString();
    std::string newWebsite = newWebsiteInput->GetValue().ToStdString();
    std::string newPassword = newPasswordInput->GetValue().ToStdString();

    if (oldWebsite.empty() || oldPassword.empty() || newWebsite.empty() || newPassword.empty()) {
        statusBox->SetValue("Fill all old and new fields.");
        return;
    }

    httplib::Client client("http://127.0.0.1:18080");
    httplib::Headers headers = {{"Content-Type", "application/json"}, {"Cookie", jwt_token}};

    std::string body = "{\"old_website\":\"" + oldWebsite + "\",\"old_password\":\"" + oldPassword +
                       "\",\"new_website\":\"" + newWebsite + "\",\"new_password\":\"" + newPassword +
                       "\",\"userId\":" + std::to_string(user_id) + "}";

    auto res = client.Post("/update_password", headers, body, "application/json");

    if (res && res->status == 200) {
        statusBox->SetValue("✅ Password updated.");
        FetchPasswords();
    } else {
        statusBox->SetValue("❌ Failed to update password.");
    }
}

void MainFrame::OnGenerate8(wxCommandEvent&) {
    httplib::Client client("http://127.0.0.1:18080");
    httplib::Headers headers = {{"Cookie", jwt_token}};
    auto res = client.Get("/generate_password/8", headers);

    if (!res || res->status != 200) {
        statusBox->SetValue("❌ Failed to generate 8-char password.");
        return;
    }

    auto json = nlohmann::json::parse(res->body);
    std::string gen_pass = json["generated_password"];

    passwordInput->SetValue(gen_pass);
    statusBox->SetValue("✅ Generated 8-char password.");
} 


void MainFrame::OnGenerate(wxCommandEvent&) {
    httplib::Client client("http://127.0.0.1:18080");
    httplib::Headers headers = {{"Cookie", jwt_token}};
    auto res = client.Get("/generate_password/16", headers);

    if (!res || res->status != 200) {
        statusBox->SetValue("❌ Failed to generate password.");
        return;
    }

    auto json = nlohmann::json::parse(res->body);
    std::string gen_pass = json["generated_password"];

    passwordInput->SetValue(gen_pass);
    statusBox->SetValue("✅ Generated password.");
}


void MainFrame::OnLogout(wxCommandEvent&) {
    httplib::Client client("http://127.0.0.1:18080");
    httplib::Headers headers = {{"Authorization", jwt_token}};
    auto res = client.Post("/logout", headers);

    if (res && res->status == 200) {
        statusBox->SetValue("✅ Logged out.");
        Close(); 
    } else {
        statusBox->SetValue("❌ Logout failed.");
    }
}

void MainFrame::OnFilter(wxCommandEvent&) {
    wxString input = wxGetTextFromUser("Enter website to filter:", "Filter");
    if (input.IsEmpty()) return;

    std::string website = input.ToStdString();

    httplib::Client client("http://127.0.0.1:18080");
    std::string url = "/filter_passwords_by_website/" + website + "?userId=" + std::to_string(user_id);
    httplib::Headers headers = {{"Cookie", jwt_token}};
    auto res = client.Get(url.c_str(), headers);

    passwordList->Clear();

    if (!res || res->status != 200) {
        statusBox->SetValue("❌ Failed to filter passwords.");
        return;
    }

    auto json = nlohmann::json::parse(res->body);
    for (auto& [website, password] : json.items()) {
        passwordList->Append(website + ": " + password.get<std::string>());
    }

    statusBox->SetValue("✅ Filtered passwords.");
}

void MainFrame::OnExport(wxCommandEvent&) {
    httplib::Client client("http://127.0.0.1:18080");

  
    std::string url = "/export_passwords_encrypted?userId=" + std::to_string(user_id);
    httplib::Headers headers = {{"Cookie", jwt_token}};
    auto res = client.Get(url.c_str(), headers);

    if (!res || res->status != 200) {
        statusBox->SetValue("❌ Export failed.");
        return;
    }

    wxFileDialog saveDialog(this, "Save Encrypted Passwords", "", "", "Encrypted Files (*.enc)|*.enc", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveDialog.ShowModal() == wxID_CANCEL) {
        statusBox->SetValue("Export canceled.");
        return;
    }

    wxFile file(saveDialog.GetPath(), wxFile::write);
    if (!file.IsOpened()) {
        statusBox->SetValue("❌ Failed to open file for writing.");
        return;
    }

    file.Write(res->body.c_str(), res->body.size());
    file.Close();

    statusBox->SetValue("✅ Exported encrypted passwords.");
}



void MainFrame::OnImport(wxCommandEvent&) {
    wxFileDialog openDialog(this, "Open Encrypted Password File", "", "", "Encrypted Files (*.enc)|*.enc", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openDialog.ShowModal() == wxID_CANCEL) return;

    wxFile file(openDialog.GetPath());
    if (!file.IsOpened()) {
        statusBox->SetValue("❌ Failed to open file.");
        return;
    }

    size_t fileSize = file.Length();
    std::vector<char> buffer(fileSize);
    file.Read(buffer.data(), fileSize);
    std::string encryptedData(buffer.data(), buffer.size());

    httplib::Client client("http://127.0.0.1:18080");
    std::string url = "/import_passwords_encrypted?userId=" + std::to_string(user_id);
    httplib::Headers headers = {{"Cookie", jwt_token}};
    auto res = client.Post(url.c_str(), headers, encryptedData, "application/octet-stream");

    if (!res || res->status != 200) {
        statusBox->SetValue("❌ Import failed.");
        return;
    }

    FetchPasswords();
    statusBox->SetValue("✅ Imported encrypted passwords.");
}
