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
    std::cout << "UI started. Server running on http://127.0.0.1:18080\n";
    std::string cmd;
    while (true) {
        std::cout << "Enter command (exit, help): ";
        std::cin >> cmd;

        if (cmd == "exit") break;
        else if (cmd == "help") {
            std::cout << "Available commands: help, exit\n";
        } else {
            std::cout << "Unknown command\n";
        }
    }
    std::cout << "UI shutting down...\n";
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
