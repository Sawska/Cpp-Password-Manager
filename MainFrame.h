#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

#include <wx/wx.h>
#include <string>

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title, const std::string& jwt, int userId);

private:

    std::string jwt_token;
    int user_id = -1;


    wxListBox* passwordList = nullptr;
    wxTextCtrl* statusBox = nullptr;

    wxTextCtrl* websiteInput = nullptr;
    wxTextCtrl* passwordInput = nullptr;

    // Update password inputs
    wxTextCtrl* oldWebsiteInput = nullptr;
    wxTextCtrl* oldPasswordInput = nullptr;
    wxTextCtrl* newWebsiteInput = nullptr;
    wxTextCtrl* newPasswordInput = nullptr;

    // Buttons
    wxButton* addButton = nullptr;
    wxButton* deleteButton = nullptr;
    wxButton* updateButton = nullptr;
    wxButton* generateButton = nullptr;

    // Logic
    void FetchPasswords();

    // Event handlers
    void OnRefresh(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnUpdate(wxCommandEvent& event);
    void OnGenerate(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif // MAIN_FRAME_H
