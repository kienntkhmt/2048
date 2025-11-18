#include "GameManager.h"
#include <iostream>

int main() {
    try {
        GameManager gm;
        gm.run();
    } catch (std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
    }
    return 0;
}
