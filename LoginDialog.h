#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H



#include <wx/wx.h>
#include <string>

class LoginDialog : public wxDialog {
public:
    LoginDialog(wxWindow* parent);

    std::string GetLogin() const;
    std::string GetPassword() const;
    bool IsRegistering() const;
     std::string GetJwtToken() const;
    int GetUserId() const;

private:
    wxTextCtrl* loginCtrl;
    wxTextCtrl* passwordCtrl;
    wxCheckBox* registerCheck;
    std::string jwtToken;
    int userId;

    void OnSubmit(wxCommandEvent&);

    std::string login;
    std::string password;
    bool registering = false;
};


#endif //LOGIN_DIALOG_H