#include "MainFrame.h"
#include "LoginDialog.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <wx/notebook.h>
#include <wx/combobox.h>
#include <wx/listbox.h>
#include <wx/statline.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>


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



namespace {

    void StyleTextCtrl(wxTextCtrl* ctrl,
                       const wxColour& normalBg = "#ffffff",
                       const wxColour& focusBg  = "#fffef7")
    {
        ctrl->SetBackgroundColour(normalBg);
        ctrl->SetForegroundColour("#333333");
        ctrl->SetWindowStyleFlag(wxBORDER_SIMPLE);

        ctrl->Bind(wxEVT_SET_FOCUS,   [=](wxFocusEvent&){ ctrl->SetBackgroundColour(focusBg); ctrl->Refresh(); });
        ctrl->Bind(wxEVT_KILL_FOCUS,  [=](wxFocusEvent&){ ctrl->SetBackgroundColour(normalBg); ctrl->Refresh(); });
        ctrl->Bind(wxEVT_ENTER_WINDOW,[=](wxMouseEvent& event){ ctrl->SetBackgroundColour(focusBg); ctrl->Refresh(); event.Skip();; });
        ctrl->Bind(wxEVT_LEAVE_WINDOW,[=](wxMouseEvent& event ){ ctrl->SetBackgroundColour(normalBg); ctrl->Refresh(); event.Skip();; });
    }


    void StyleButton(wxButton* btn,
                     const wxColour& fg,
                     const wxColour& bg,
                     const wxColour& hover)
    {
        btn->SetForegroundColour(fg);
        btn->SetBackgroundColour(bg);
        btn->SetWindowStyle(wxBORDER_SIMPLE);

        btn->Bind(wxEVT_ENTER_WINDOW, [=](wxMouseEvent&){ btn->SetBackgroundColour(hover); btn->Refresh(); });
        btn->Bind(wxEVT_LEAVE_WINDOW, [=](wxMouseEvent&){ btn->SetBackgroundColour(bg);    btn->Refresh(); });
    }

    wxStaticText* MakeLabel(wxWindow* parent,
                            const wxString& text,
                            const wxFont& font,
                            const wxColour& colour = "#333333")
    {
        auto* l = new wxStaticText(parent, wxID_ANY, text);
        l->SetFont(font);
        l->SetForegroundColour(colour);
        return l;
    }
}



MainFrame::MainFrame(const wxString& title,
                     const std::string& jwt,
                     int userId)
    : wxFrame(nullptr, wxID_ANY, title,
              wxDefaultPosition, wxSize(1920, 1080),
              wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN),
      jwt_token(jwt),
      user_id(userId)
{
    const wxColour accent        ("#1976D2");
    const wxColour accentHover   ("#1E88E5");
    const wxColour danger        ("#D32F2F");
    const wxColour dangerHover   ("#E53935");
    const wxColour secondaryText ("#555555");

    wxFont titleFont (12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    wxFont labelFont (11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_MEDIUM);
    wxFont inputFont (10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont buttonFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    wxFont monoFont  ( 9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    auto* panel = new wxPanel(this);
    panel->SetBackgroundColour("#f0f0f0");

    if (jwt_token.empty()) {
        LoginDialog dlg(this);
        if (dlg.ShowModal() != wxID_OK) { Close(); return; }

        httplib::Client           client("http://127.0.0.1:18080");
        httplib::Headers          headers{{"Content-Type","application/json"}};
        std::string               body = "{\"login\":\""+dlg.GetLogin()+"\",\"password\":\""+dlg.GetPassword()+"\"}";
        const std::string         ep   = dlg.IsRegistering()?"/register":"/login";
        if (auto res = client.Post(ep.c_str(),headers,body,"application/json"); !res || res->status!=200) {
            wxMessageBox("Login/Register failed","Error",wxICON_ERROR); Close(); return;
        } else {
            jwt_token = res->get_header_value("Set-Cookie");
            user_id   = nlohmann::json::parse(res->body)["Userid"];
        }
    }

    auto* notebook = new wxNotebook(panel, wxID_ANY);
    notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, [=](wxBookCtrlEvent& e) {
        for (int i=0;i<notebook->GetPageCount();++i) {
            notebook->GetPage(i)->SetBackgroundColour(i==e.GetSelection()?"#fdfdfd":"#ffffff");
            notebook->GetPage(i)->Refresh();
        }
    });

    auto* addPg = new wxPanel(notebook); addPg->SetBackgroundColour("#ffffff");
    auto* addBox = new wxStaticBox(addPg, wxID_ANY, "➕  Add New Password"); addBox->SetFont(titleFont);
    auto* addSizer = new wxStaticBoxSizer(addBox, wxVERTICAL);
    auto* addGrid  = new wxFlexGridSizer(2,10,10); addGrid->AddGrowableCol(1,1);

    websiteInput  = new wxTextCtrl(addPg, wxID_ANY, "", wxDefaultPosition, wxSize(250,28));
    passwordInput = new wxTextCtrl(addPg, wxID_ANY, "", wxDefaultPosition, wxSize(250,28));
    websiteInput ->SetFont(inputFont);
    passwordInput->SetFont(inputFont);
    StyleTextCtrl(websiteInput); StyleTextCtrl(passwordInput);

    auto* addBtn = new wxButton(addPg, 1002, "💾  Save"); addBtn->SetFont(buttonFont);
    StyleButton(addBtn,  *wxBLACK, accent, accentHover);

    addGrid->Add(MakeLabel(addPg,"🌐  Website:", labelFont), 0, wxALIGN_CENTER_VERTICAL);
    addGrid->Add(websiteInput, 1, wxEXPAND);
    addGrid->Add(MakeLabel(addPg,"🔒  Password:", labelFont),0, wxALIGN_CENTER_VERTICAL);
    addGrid->Add(passwordInput,1, wxEXPAND);
    addGrid->AddSpacer(1);
    addGrid->Add(addBtn,0,wxALIGN_RIGHT|wxTOP,5);
    addSizer->Add(addGrid,1,wxALL|wxEXPAND,10);
    addPg->SetSizer(addSizer);

    auto* updPg = new wxPanel(notebook); updPg->SetBackgroundColour("#ffffff");
    auto* updBox = new wxStaticBox(updPg, wxID_ANY, "✏️  Update / Delete"); updBox->SetFont(titleFont);
    auto* updSizer = new wxStaticBoxSizer(updBox, wxVERTICAL);
    auto* updGrid  = new wxFlexGridSizer(2,10,10); updGrid->AddGrowableCol(1,1);

    auto* selLabel = MakeLabel(updPg,"Select entry in list above",labelFont,secondaryText);
    newWebsiteInput  = new wxTextCtrl(updPg, wxID_ANY, "", wxDefaultPosition, wxSize(250,28));
    newPasswordInput = new wxTextCtrl(updPg, wxID_ANY, "", wxDefaultPosition, wxSize(250,28));
    newWebsiteInput ->SetFont(inputFont);
    newPasswordInput->SetFont(inputFont);
    StyleTextCtrl(newWebsiteInput); StyleTextCtrl(newPasswordInput);

    auto* updBtn = new wxButton(updPg,1004,"🔄  Update"); updBtn->SetFont(buttonFont);
    auto* delBtn = new wxButton(updPg,1003,"🗑️  Delete"); delBtn->SetFont(buttonFont);
    StyleButton(updBtn,*wxBLACK,accent,accentHover);
    StyleButton(delBtn,*wxBLACK,danger,dangerHover);

    updGrid->Add(selLabel,0,wxBOTTOM,5); updGrid->AddSpacer(1);
    updGrid->Add(MakeLabel(updPg,"🔄  New Website:",labelFont),0,wxALIGN_CENTER_VERTICAL);
    updGrid->Add(newWebsiteInput,1,wxEXPAND);
    updGrid->Add(MakeLabel(updPg,"🔑  New Password:",labelFont),0,wxALIGN_CENTER_VERTICAL);
    updGrid->Add(newPasswordInput,1,wxEXPAND);
    updGrid->Add(delBtn,0,wxALIGN_LEFT); updGrid->Add(updBtn,0,wxALIGN_RIGHT);
    updSizer->Add(updGrid,1,wxALL|wxEXPAND,10); updPg->SetSizer(updSizer);


    auto* filtPg = new wxPanel(notebook); filtPg->SetBackgroundColour("#ffffff");
    auto* filtSizer = new wxBoxSizer(wxVERTICAL);
    auto* filtBtn = new wxButton(filtPg,1007,"🔎  Filter by Website"); filtBtn->SetFont(buttonFont);
    StyleButton(filtBtn,*wxBLACK,accent,accentHover);
    filtSizer->Add(filtBtn,0,wxALL|wxALIGN_CENTER,10); filtPg->SetSizer(filtSizer);

    notebook->AddPage(addPg,"Add");
    notebook->AddPage(updPg,"Update/Delete");
    notebook->AddPage(filtPg,"Filter");

    passwordList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition,
                                      wxSize(-1,160),
                                      wxLC_REPORT | wxBORDER_SIMPLE);
    passwordList->InsertColumn(0,"Website",  wxLIST_FORMAT_LEFT, 300);
    passwordList->InsertColumn(1,"Password", wxLIST_FORMAT_LEFT, 300);
    passwordList->SetFont(inputFont);
    passwordList->SetBackgroundColour("#ffffff");
    passwordList->SetTextColour("#222222");

    auto* actions = new wxBoxSizer(wxHORIZONTAL);
    auto makeActBtn=[&](int id,const wxString& txt,const wxColour& fg="#1976D2",const wxColour& bg="#f9f9f9",const wxColour& hov="#e3f2fd"){
        auto* b=new wxButton(panel,id,txt); b->SetFont(buttonFont); StyleButton(b,fg,bg,hov); return b;
    };
    actions->Add(makeActBtn(1010,"🔒  Gen 8‑char","#FFFFFF",accent,accentHover),0,wxALL,5);
    actions->Add(makeActBtn(1005,"🔑  Gen 16‑char","#FFFFFF",accent,accentHover),0,wxALL,5);
    actions->Add(makeActBtn(1008,"📤  Export",accent),0,wxALL,5);
    actions->Add(makeActBtn(1009,"📥  Import",accent),0,wxALL,5);
    actions->Add(makeActBtn(1006,"🚪  Logout","#FFFFFF",danger,dangerHover),0,wxALL,5);
    actions->Add(makeActBtn(1001,"🔁  Refresh",accent),0,wxALL,5);

    statusBox = new wxTextCtrl(panel,wxID_ANY,"",wxDefaultPosition,wxSize(-1,90),
                               wxTE_MULTILINE|wxTE_READONLY|wxBORDER_SIMPLE);
    statusBox->SetFont(monoFont);
    statusBox->SetBackgroundColour("#eeeeee");
    statusBox->SetForegroundColour("#333333");

    auto* top = new wxBoxSizer(wxVERTICAL);
    top->Add(notebook,        0, wxEXPAND|wxALL,10);
    top->Add(passwordList,1, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM,10);
    top->Add(actions,         0, wxALIGN_CENTER|wxBOTTOM,5);
    top->Add(statusBox,       0, wxEXPAND|wxALL,10);
    panel->SetSizer(top);
    top->SetSizeHints(this);

    websiteInput     ->SetToolTip("Enter the website or service name");
    passwordInput    ->SetToolTip("Enter your password for this site");
    newWebsiteInput  ->SetToolTip("New website name");
    newPasswordInput ->SetToolTip("New password value");
    addBtn           ->SetToolTip("Save this password");
    updBtn           ->SetToolTip("Update selected entry");
    delBtn           ->SetToolTip("Delete selected entry");
    filtBtn          ->SetToolTip("Filter list by website");

    FetchPasswords();
}

    void MainFrame::FetchPasswords() {
        httplib::Client client("http://127.0.0.1:18080");
        std::string url = "/get_passwords?userId=" + std::to_string(user_id);
        httplib::Headers headers = {{"Cookie", jwt_token}};
        auto res = client.Get(url.c_str(), headers);

        passwordList->DeleteAllItems();


        if (!res || res->status != 200) {
            statusBox->SetValue("❌ Failed to fetch passwords.");
            return;
        }

        auto json = nlohmann::json::parse(res->body);
        for (auto& [website, password] : json.items()) {
            long index = passwordList->InsertItem(0, website); 
            passwordList->SetItem(index, 1, password.get<std::string>());  

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
        long sel = passwordList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
if (sel == -1) {
    statusBox->SetValue("Select a password to delete.");
    return;
}


        wxString websiteWx = passwordList->GetItemText(sel); 

wxListItem item;
item.SetId(sel);
item.SetColumn(1); 
passwordList->GetItem(item);
wxString passwordWx = item.GetText();

std::string selected = websiteWx.ToStdString() + ": " + passwordWx.ToStdString();

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
   long sel = passwordList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
if (sel == -1) {
    statusBox->SetValue("Select a password to delete.");
    return;
}

    wxString website = passwordList->GetItemText(sel); 

wxListItem item;
item.SetId(sel);
item.SetColumn(1); 
passwordList->GetItem(item);
wxString password = item.GetText();
    std::string selected = website.ToStdString() + ": " + password.ToStdString();
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

        passwordList->DeleteAllItems();


        if (!res || res->status != 200) {
            statusBox->SetValue("❌ Failed to filter passwords.");
            return;
        }

        auto json = nlohmann::json::parse(res->body);
        for (auto& [website, password] : json.items()) {
    long index = passwordList->InsertItem(passwordList->GetItemCount(), website);
    passwordList->SetItem(index, 1, password.get<std::string>());
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
