#include "Brainiac.hpp"

namespace Brainiac {
    void init() {
        ROOK_ATTACK_TABLES = Brainiac::init_rook_tables();
        BISHOP_ATTACK_TABLES = Brainiac::init_bishop_tables();
    }
} // namespace Brainiac
