#pragma once
#include <algorithm>
#include <cstring>
#include <span>
#include <vector>
#include "sortnet/sorting_network.hpp"

// inline void merge_insertion_sort(int* arr, size_t len) noexcept {
//     for (size_t i = 1; i < len; ++i) {
//         int key = arr[i]; size_t j = i;
//         while (j > 0 && arr[j-1] > key) arr[j--] = arr[j-1];
//         arr[j] = key;
//     }
// }

[[gnu::hot]]
static void merge_core(int* __restrict src, int* __restrict dst, size_t left, size_t right) noexcept{

    size_t len = right - left;
    int* out = dst + left;
    int* in = src + left;

    // Случай малого массива
    if (len <= 64){
        std::memcpy(out, in, len * sizeof(int));
        network_sort(out, len); return;
    }

    // if (len <= 64){
    //     std::memcpy(out, in, len * sizeof(int));
    //     merge_insertion_sort(out, len);
    //     return;
    // }

    // Ping-pong сортировка (в два раза меньше копирований)
    size_t mid = left + len / 2;
    merge_core(dst, src, left, mid);
    merge_core(dst, src, mid, right);

    // Массивы могут быть уже склеены
    if (src[mid - 1] <= src[mid]) [[unlikely]] {
        std::memcpy(dst + left, src + left, len * sizeof(int)); return;
    }

    // Копирование
    const int* l = src + left, *le = src + mid;
    const int* r = src + mid, *re = src + right;

    // надо попробовать std::merge
    while (l < le && r < re) {
        bool pick = (*l <= *r);
        *out++ = std::min(*l, *r);
        l += pick;
        r += !pick;
    }
    // Докопирование оставшегося куска
    size_t rl = (size_t)(le - l), rr = (size_t)(re - r);
    std::memcpy(out, l, rl * sizeof(int));
    std::memcpy(out + rl, r, rr * sizeof(int));
}

inline void merge_sort(std::span<int> arr) noexcept{
    if (arr.size() <= 1) return;
    std::vector<int> buffer(arr.begin(), arr.end());
    merge_core(buffer.data(), arr.data(), 0, arr.size());
}