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

void quick_sort_impl(int* first, int* last) noexcept{
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
    
    size_t len = last - first;
    switch(len){
        case 0: case 1: return;
        case 2: cmp(first[0], first[1]); return;
        case 3: sort3(first); return;
        case 4: sort4(first); return;
        case 5: sort5(first); return;
        case 6: sort6(first); return;
        case 7: sort7(first); return;
        case 8: sort8(first); return;
        case 9: sort9(first); return;
        case 10: sort10(first); return;
        case 11: sort11(first); return;
        case 12: sort12(first); return;
        case 13: sort13(first); return;
        case 14: sort14(first); return;
        case 15: sort15(first); return;
        case 16: sort16(first); return;
        case 17: sort17(first); return;
        case 18: sort18(first); return;
        case 19: sort19(first); return;
        case 20: sort20(first); return;
        case 21: sort21(first); return;
        case 22: sort22(first); return;
        case 23: sort23(first); return;
        case 24: sort24(first); return;
        case 25: sort25(first); return;
        case 26: sort26(first); return;
        case 27: sort27(first); return;
        case 28: sort28(first); return;
        case 29: sort29(first); return;
        case 30: sort30(first); return;
        case 31: sort31(first); return;
        case 32: sort32(first); return;
        case 33: sort33(first); return;
        case 34: sort34(first); return;
        case 35: sort35(first); return;
        case 36: sort36(first); return;
        case 37: sort37(first); return;
        case 38: sort38(first); return;
        case 39: sort39(first); return;
        case 40: sort40(first); return;
        case 41: sort41(first); return;
        case 42: sort42(first); return;
        case 43: sort43(first); return;
        case 44: sort44(first); return;
        case 45: sort45(first); return;
        case 46: sort46(first); return;
        case 47: sort47(first); return;
        case 48: sort48(first); return;
        case 49: sort49(first); return;
        case 50: sort50(first); return;
        case 51: sort51(first); return;
        case 52: sort52(first); return;
        case 53: sort53(first); return;
        case 54: sort54(first); return;
        case 55: sort55(first); return;
        case 56: sort56(first); return;
        case 57: sort57(first); return;
        case 58: sort58(first); return;
        case 59: sort59(first); return;
        case 60: sort60(first); return;
        case 61: sort61(first); return;
        case 62: sort62(first); return;
        case 63: sort63(first); return;
        case 64: sort64(first); return;
    }
}

void quick_sort(std::span<int> arr) noexcept{
    if (arr.size() <= 1) return;
    quick_sort_impl(arr.data(), arr.data() + arr.size());
}