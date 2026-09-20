#include "shadowdeep/core/rng.hpp"
#include <cassert>

void test_rng_determinism() {
    shadowdeep::Rng a(12345);
    shadowdeep::Rng b(12345);
    for (int i = 0; i < 100; ++i) {
        assert(a.range(0, 100) == b.range(0, 100));
    }
}
