#pragma once
#include <span>

// Time complexity: 
// Best: O(n)
// Average: O(n^2)
// Worst: O(n^2)
// Space complexity: Θ(1)
void insertion_sort(std::span<int> arr) noexcept{
    const size_t n = arr.size();
    if (n <= 1) return;
    int* const data = arr.data();

    for (size_t i = 1; i < n; ++i){
        const int x = data[i];
        size_t j = i;
        while (j > 0 && data[j - 1] > x) data[j--] = data[j - 1];
        data[j] = x;
    }
}