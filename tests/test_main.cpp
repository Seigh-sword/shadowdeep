#include <iostream>

void test_rng_determinism();
void test_dungeon_connectivity();
void test_save_roundtrip();
void test_paths();
void test_combat();

int main() {
    std::cout << "Running shadowdeep tests\n";
    try {
        test_rng_determinism();
        std::cout << "rng determinism ok\n";
        test_dungeon_connectivity();
        std::cout << "dungeon connectivity ok\n";
        test_save_roundtrip();
        std::cout << "save roundtrip ok\n";
        test_paths();
        std::cout << "paths ok\n";
        test_combat();
        std::cout << "combat ok\n";
        std::cout << "All tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    }
}
