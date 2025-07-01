#include "GuiApp.h"
#include "MainFrame.h"
#include "LoginDialog.h"

#include <wx/wx.h>

// wxIMPLEMENT_APP(GuiApp);

bool GuiApp::OnInit() {
    LoginDialog loginDlg(nullptr);
    if (loginDlg.ShowModal() != wxID_OK) {
        return false; 
    }

    std::string jwt = loginDlg.GetJwtToken();
    int userId = loginDlg.GetUserId();

    MainFrame* frame = new MainFrame("Password Manager", jwt, userId);
    frame->Show(true);
    return true;
}
