#include "shadowdeep/platform/paths.hpp"
#include <cassert>

void test_paths() {
    auto p = shadowdeep::getAppPaths();
    assert(!p.dataRoot.empty());
    assert(!p.savesDir.empty());
}
