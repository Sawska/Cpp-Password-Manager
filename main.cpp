#include "AppManager.h"
#include <iostream>

int main(int argc, char* argv[]) {
    AppMode mode = AppMode::UI; 
    if (argc > 1 && std::string(argv[1]) == "web") {
        mode = AppMode::WEB;
    }

    AppManager manager(mode);
    manager.run();

    return 0;
}