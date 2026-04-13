#pragma once
#include <span>
#include <utility>
#include "sortnet/sorting_network.hpp"

// void quick_insertion_sort_ptr(int* first, int* last) noexcept{
//     if (last - first <= 1) return;
//     for (int* i = first + 1; i < last; ++i) {
//         const int val = *i;
//         int* j = i;
//         while (j > first && *(j - 1) > val) {
//             *j-- = *(j - 1);
//         }
//         *j = val;
//     }
// }

[[gnu::hot]]
int* partition(int* first, int* last) noexcept{
    const int* const mid = first + (last - first) / 2;
    const int a = *first, b = *mid, c = *(last - 1);

    const int pivot = a < b ? (b < c ? b : (a < c ? c : a)) 
                      : (a < c ? a : (b < c ? c : b));

    int* left = first;
    int* right = last - 1;

    while (true) {
        while (*left < pivot) ++left;
        while (*right > pivot) --right;
        
        if (left >= right) return right;

        std::swap(*left, *right);
        ++left;
        --right;
    }
}

void quick_sort_impl(int* __restrict first, int* __restrict last) noexcept{
    while (last - first > 64) {
        int* p = partition(first, last);
        
        // p - это последний элемент левого подмассива.
        // Левая часть: [first, p + 1)
        // Правая часть: [p + 1, last)

        // Всегда делаем рекурсивный вызов для МЕНЬШЕЙ части,
        if (p + 1 - first < last - (p + 1)) {
            quick_sort_impl(first, p + 1); // Рекурсия в меньшую левую
            first = p + 1;                 // Цикл переходит на большую правую
        } else {
            quick_sort_impl(p + 1, last);  // Рекурсия в меньшую правую
            last = p + 1;                  // Цикл переходит на большую левую
        }
    }
    
    // quick_insertion_sort_ptr(first, last);
    size_t len = last - first;
    network_sort(first, len);
}

void quick_sort(std::span<int> arr) noexcept{
    if (arr.size() <= 1) return;
    quick_sort_impl(arr.data(), arr.data() + arr.size());
}