#include <cstddef>
#include "group.hpp"
int main() {
    
    // Количество бассейнов - N = 38 миллион
    // Количество каналов K = 11 миллионов
    // Количество добавлений воды L = 29 миллионов
    // Количество разрывов M = 3 миллиона
    // Количество добавляемой воды в каждый бассейн от 1 литра до 500.
    const int32_t N = 5;
    const int32_t K = 11;
    const int32_t L = 2;
    const int32_t M = 3;
    Groups groups{N};

    for (int32_t i = 0; i < N; ++i) {
        groups.add_group(i, static_cast<double>(i + 2));
    }
    
}