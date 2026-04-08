#pragma once
#include <algorithm>
#include <span>
#include <vector>

inline void merge_insertion_sort_ptr(int* first, int* last) noexcept{
    if (last - first <= 1) return;
    for (int* i = first + 1; i < last; ++i) {
        const int val = *i;
        int* j = i;
        while (j > first && *(j - 1) > val) *j-- = *(j - 1);
        *j = val;
    }
}

inline void merge_core(int* src, int* dst, size_t left, size_t right) noexcept{
    
    // Случай малого массива
    if (right - left <= 32){
        std::copy(src + left, src + right, dst + left);
        merge_insertion_sort_ptr(dst + left, dst + right);
        return;
    }

    // Ping-pong сортировка (в два раза меньше копирований)
    size_t mid = left + (right - left) / 2;
    merge_core(dst, src, left, mid);
    merge_core(dst, src, mid, right);

    // Массивы могут быть уже склеены
    if (src[mid - 1] <= src[mid]){
        std::copy(src + left, src + right, dst + left);
    }

    // Копирование
    const int* l1 = src + left;
    const int* l2 = src + mid;
    const int* r1 = src + mid;
    const int* r2 = src + right;
    int* out = dst + left;

    // надо попробовать std:merge
    while (l1 < r1 && l2 < r2) *out++ = (*l1 <= *l2) ? *l1++ : *l2++;
    // Докопирование оставшегося куска
    // while (l1 < r1) *out++ = *l1++;
    // while (l2 < r2) *out++ = *l2++;
    if (l1 < r1) std::copy(l1, r1, out);
    if (l2 < r2) std::copy(l2, r2, out);
}

inline void merge_sort(std::span<int> arr) noexcept{
    if (arr.size() <= 1) return;
    std::vector<int> buffer(arr.begin(), arr.end());
    merge_core(buffer.data(), arr.data(), 0, arr.size());
}