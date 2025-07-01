#include "AppManager.h"
#include <iostream>
#include <chrono>
#include <thread>


AppManager::AppManager(AppMode mode) : mode(mode), server(mode) {}

AppManager::~AppManager() {
    if (serverThread.joinable()) {
        serverThread.detach();
    }
}

void AppManager::run_server() {
    server.runServer();
}



void AppManager::run_ui() {
    int argc = 0;
    char** argv = nullptr;

    wxApp::SetInstance(new GuiApp());
    wxEntryStart(argc, argv);
    wxTheApp->CallOnInit();
    wxTheApp->OnRun();
    wxEntryCleanup();
}




void AppManager::run() {
    if (mode == AppMode::WEB) {
        server.runServer();
    } else if (mode == AppMode::UI) {
        serverThread = std::thread([this]() {
            this->run_server();
        });
        run_ui();
    }
}
