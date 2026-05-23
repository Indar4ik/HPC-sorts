#pragma once
#include <span>
#include <cstdint>

// Time complexity: 
// Best: O(n)
// Average: O(n^2)
// Worst: O(n^2)
// Space complexity: Θ(1)
void insertion_sort(std::span<int> arr) noexcept{
    const uint64_t n = arr.size();
    if (n <= 1) return;
    int* const data = arr.data();

    for (uint64_t i = 1; i < n; ++i){
        const int x = data[i];
        uint64_t j = i;
        while (j > 0 && data[j - 1] > x) data[j--] = data[j - 1];
        data[j] = x;
    }
}