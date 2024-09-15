#include "Engine.hpp"

namespace Brainiac {
    void init() {
        ROOK_ATTACK_TABLES = Brainiac::init_rook_tables();
        BISHOP_ATTACK_TABLES = Brainiac::init_bishop_tables();
    }

    std::string get_engine_version() { return "v1.0"; }
} // namespace Brainiac
