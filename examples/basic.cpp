#include "skiplist.h"

int main() {
    // Simple configuration with int, using the default amount of maximum layers (20) and std::less as the comparison functor for the type stored
    SkipList<int> slist;

    slist.Insert(-5);
    slist.Insert(42);

    slist.Print();

    if (slist.Find(42)) {
        std::cout << "Found Answer to the Ultimate Question of Life, the Universe, and Everything!\n";
    }

    slist.Remove(42);

    if (!slist.Find(42)) {
        std::cout << "Answer is not here anymore :(\n";
    }

    // Custom configuration with std::string, fixing maximum layer amount at 8 and reversing the order of comparison
    SkipList<std::string, 8, std::greater<>> strlist{"fefo", "phef", "outer", "wilds"};

    for (auto& s : strlist) {
        std::cout << s << "\n";
    }
}