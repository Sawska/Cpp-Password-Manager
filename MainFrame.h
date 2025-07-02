#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

#include <wx/wx.h>
#include <string>
#include <wx/wfstream.h>
#include <wx/file.h>


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


    wxTextCtrl* oldWebsiteInput = nullptr;
    wxTextCtrl* oldPasswordInput = nullptr;
    wxTextCtrl* newWebsiteInput = nullptr;
    wxTextCtrl* newPasswordInput = nullptr;


    wxButton* addButton = nullptr;
    wxButton* deleteButton = nullptr;
    wxButton* updateButton = nullptr;
    wxButton* generateButton = nullptr;


    void FetchPasswords();


    void OnRefresh(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnUpdate(wxCommandEvent& event);
    void OnGenerate(wxCommandEvent& event);
    void OnLogout(wxCommandEvent&);
    void OnFilter(wxCommandEvent&);
    void OnExport(wxCommandEvent&);
    void OnImport(wxCommandEvent&);
    void OnGenerate8(wxCommandEvent&);

    DECLARE_EVENT_TABLE()
};

#endif // MAIN_FRAME_H
