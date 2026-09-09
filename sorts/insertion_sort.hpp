#pragma once
#include <span>
#include <cstdint>

namespace insertion_detail {

template <typename T>
inline void run(T* const data, const uint64_t n) noexcept{
    for (uint64_t i = 1; i < n; ++i){
        const T x = data[i];
        uint64_t j = i;
        while (j > 0 && data[j - 1] > x) data[j--] = data[j - 1];
        data[j] = x;
    }
}

} // namespace insertion_detail

// Time complexity:
// Best: O(n)
// Average: O(n^2)
// Worst: O(n^2)
// Space complexity: Θ(1)
inline void insertion_sort(std::span<int32_t> arr) noexcept{
    if (arr.size() <= 1) return;
    insertion_detail::run(arr.data(), arr.size());
}

inline void insertion_sort(std::span<uint32_t> arr) noexcept{
    if (arr.size() <= 1) return;
    insertion_detail::run(arr.data(), arr.size());
}
