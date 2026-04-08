#include <span>
#include <concepts>
#include <algorithm>
#include <utility>
#include <future>
#include <bit>

namespace hpc {

template <typename T>
concept SortableInt = std::same_as<T, int>;

inline constexpr size_t CUTOFF = 64;

[[nodiscard]] constexpr int median_of_3(int a, int b, int c) noexcept {
    int min_ab = (a < b) ? a : b;
    int max_ab = (a > b) ? a : b;
    int min_max_ab_c = (max_ab < c) ? max_ab : c;
    return (min_ab > min_max_ab_c) ? min_ab : min_max_ab_c;
}

template <std::contiguous_iterator It>
requires SortableInt<std::iter_value_t<It>>
[[nodiscard]] It branchless_partition(It first, It last) noexcept {
    const size_t len = last - first;
    [[assume(len >= 2)]]; 

    auto mid = first + len / 2;
    int pivot = median_of_3(*first, *mid, *(last - 1));
    if (*mid == pivot) std::swap(*mid, *(last - 1));
    else if (*first == pivot) std::swap(*first, *(last - 1));

    It left = first;
    int toggle = 0;

    for (It right = first; right < last - 1; ++right) {
        int val = *right;
        int tmp = *left;

        int is_eq = (val == pivot);
        int is_less = (val < pivot) | (is_eq & toggle);
        toggle ^= is_eq;

        *right = is_less ? tmp : val;
        *left  = is_less ? val : tmp;
        
        left += is_less;
    }

    std::swap(*left, *(last - 1));
    return left;
}

void quick_sort_serial(std::span<int> arr) noexcept {
    if (arr.size() <= CUTOFF) {
        std::sort(arr.begin(), arr.end());
        return;
    }

    auto pivot_it = branchless_partition(arr.begin(), arr.end());
    size_t left_size = pivot_it - arr.begin();

    quick_sort_serial(arr.subspan(0, left_size));
    quick_sort_serial(arr.subspan(left_size + 1));
}

void quick_sort_parallel(std::span<int> arr, int depth = 0) {
    if (arr.size() <= CUTOFF) {
        std::sort(arr.begin(), arr.end());
        return;
    }

    if (depth > 8 || arr.size() < 100'000) {
        quick_sort_serial(arr);
        return;
    }

    auto pivot_it = branchless_partition(arr.begin(), arr.end());
    size_t left_size = pivot_it - arr.begin();

    auto future_right = std::async(std::launch::async, quick_sort_parallel, arr.subspan(left_size + 1), depth + 1);
    
    quick_sort_parallel(arr.subspan(0, left_size), depth + 1);
    
    future_right.wait();
}

}