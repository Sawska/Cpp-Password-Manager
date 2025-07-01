#pragma once
#include "Mode.h"
#include "Server.h"
#include <thread>
#include "GuiApp.h"

class AppManager {
public:
    AppManager(AppMode mode);
    ~AppManager();

    void run();

private:
    AppMode mode;
    Server server;
    std::thread serverThread;

    void run_server();
    void run_ui();
};
