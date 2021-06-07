#include "random.h"

namespace chess::neural {
    double random() {
        return uniform(rng);
    }

    int randrange(int start, int stop) {
        return random() * (stop - start) + start;
    }
}