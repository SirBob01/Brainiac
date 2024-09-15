#include "Engine.hpp"

int main(int argc, char *argv[]) {
    Brainiac::init();
    Brainiac::UCI uci;
    uci.run();
    return 0;
}