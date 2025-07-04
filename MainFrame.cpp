    #include "MainFrame.h"
    #include "LoginDialog.h"
    #include <httplib.h>
    #include <nlohmann/json.hpp>
    #include <wx/notebook.h>
    #include <wx/combobox.h>


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
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(900, 650)),
        jwt_token(jwt),
        user_id(userId) {

        wxPanel* panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        
    if (jwt.empty()) {
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
    } else {
        jwt_token = jwt;
        user_id = userId;
    }


        
        wxNotebook* notebook = new wxNotebook(panel, wxID_ANY);
        
        
        wxPanel* addPanel = new wxPanel(notebook);
        wxFlexGridSizer* addSizer = new wxFlexGridSizer(2, 10, 10);
        addSizer->AddGrowableCol(1, 1);

        websiteInput = new wxTextCtrl(addPanel, wxID_ANY, "", wxDefaultPosition, wxSize(250, -1));
        passwordInput = new wxTextCtrl(addPanel, wxID_ANY, "", wxDefaultPosition, wxSize(250, -1));
        wxButton* addButton = new wxButton(addPanel, 1002, "Add Password");

        addSizer->Add(new wxStaticText(addPanel, wxID_ANY, "Website:"), 0, wxALIGN_CENTER_VERTICAL);
        addSizer->Add(websiteInput, 1, wxEXPAND);
        addSizer->Add(new wxStaticText(addPanel, wxID_ANY, "Password:"), 0, wxALIGN_CENTER_VERTICAL);
        addSizer->Add(passwordInput, 1, wxEXPAND);
        addSizer->AddStretchSpacer();
        addSizer->Add(addButton, 0, wxALIGN_RIGHT);
        addPanel->SetSizer(addSizer);

        
        wxPanel* updatePanel = new wxPanel(notebook);
        wxFlexGridSizer* updateSizer = new wxFlexGridSizer(2, 10, 10);
        updateSizer->AddGrowableCol(1, 1);

    wxStaticText* selLabel = new wxStaticText(updatePanel, wxID_ANY, "Select Entry in list above");

        newWebsiteInput = new wxTextCtrl(updatePanel, wxID_ANY, "", wxDefaultPosition, wxSize(250, -1));
        newPasswordInput = new wxTextCtrl(updatePanel, wxID_ANY, "", wxDefaultPosition, wxSize(250, -1));
        wxButton* updateButton = new wxButton(updatePanel, 1004, "Update Password");
        deleteButton = new wxButton(updatePanel, 1003, "Delete This Password");

updateSizer->Add(selLabel, 0, wxALIGN_CENTER_VERTICAL);
updateSizer->AddStretchSpacer(); 

        updateSizer->Add(new wxStaticText(updatePanel, wxID_ANY, "New Website:"), 0, wxALIGN_CENTER_VERTICAL);
        updateSizer->Add(newWebsiteInput, 1, wxEXPAND);
        updateSizer->Add(new wxStaticText(updatePanel, wxID_ANY, "New Password:"), 0, wxALIGN_CENTER_VERTICAL);
        updateSizer->Add(newPasswordInput, 1, wxEXPAND);
        updateSizer->Add(deleteButton, 0, wxALIGN_LEFT);
        updateSizer->Add(updateButton, 0, wxALIGN_RIGHT);
        updatePanel->SetSizer(updateSizer);

        
        wxPanel* filterPanel = new wxPanel(notebook);
        wxBoxSizer* filterSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton* filterBtn = new wxButton(filterPanel, 1007, "Filter By Website");
        filterSizer->Add(filterBtn, 0, wxALL | wxALIGN_CENTER, 10);
        filterPanel->SetSizer(filterSizer);

        
        notebook->AddPage(addPanel, "Add");
        notebook->AddPage(updatePanel, "Update/Delete");
        notebook->AddPage(filterPanel, "Filter");

        
        passwordList = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        passwordList->SetMinSize(wxSize(-1, 150));

        
        wxBoxSizer* actions = new wxBoxSizer(wxHORIZONTAL);
        actions->Add(new wxButton(panel, 1010, "Gen 8-char"), 0, wxALL, 5);
        actions->Add(new wxButton(panel, 1005, "Gen 16-char"), 0, wxALL, 5);
        actions->Add(new wxButton(panel, 1008, "Export"), 0, wxALL, 5);
        actions->Add(new wxButton(panel, 1009, "Import"), 0, wxALL, 5);
        actions->Add(new wxButton(panel, 1006, "Logout"), 0, wxALL, 5);
        actions->Add(new wxButton(panel, 1001, "Refresh"), 0, wxALL, 5);

        
        statusBox = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 80), wxTE_MULTILINE | wxTE_READONLY);

        
        mainSizer->Add(notebook,      0, wxEXPAND | wxALL, 10);
        mainSizer->Add(passwordList,  1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
        mainSizer->Add(actions,       0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
        mainSizer->Add(statusBox,     0, wxEXPAND | wxALL, 10);

        panel->SetSizer(mainSizer);

        
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
    int sel = passwordList->GetSelection();
    if (sel == wxNOT_FOUND) {
        statusBox->SetValue("Select a password to update.");
        return;
    }

    std::string selected = passwordList->GetString(sel).ToStdString();
    size_t colon = selected.find(':');
    if (colon == std::string::npos) {
        statusBox->SetValue("Invalid format.");
        return;
    }

    std::string oldWebsite = selected.substr(0, colon);
    std::string oldPassword = selected.substr(colon + 2);

    std::string newWebsite = newWebsiteInput->GetValue().ToStdString();
    std::string newPassword = newPasswordInput->GetValue().ToStdString();

    if (newWebsite.empty() || newPassword.empty()) {
        statusBox->SetValue("Please enter new website and password.");
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
