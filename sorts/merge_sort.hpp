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
        switch(len){
            case 0: case 1: return;
            case 2: cmp(out[0], out[1]); return;
            case 3: sort3(out); return;
            case 4: sort4(out); return;
            case 5: sort5(out); return;
            case 6: sort6(out); return;
            case 7: sort7(out); return;
            case 8: sort8(out); return;
            case 9: sort9(out); return;
            case 10: sort10(out); return;
            case 11: sort11(out); return;
            case 12: sort12(out); return;
            case 13: sort13(out); return;
            case 14: sort14(out); return;
            case 15: sort15(out); return;
            case 16: sort16(out); return;
            case 17: sort17(out); return;
            case 18: sort18(out); return;
            case 19: sort19(out); return;
            case 20: sort20(out); return;
            case 21: sort21(out); return;
            case 22: sort22(out); return;
            case 23: sort23(out); return;
            case 24: sort24(out); return;
            case 25: sort25(out); return;
            case 26: sort26(out); return;
            case 27: sort27(out); return;
            case 28: sort28(out); return;
            case 29: sort29(out); return;
            case 30: sort30(out); return;
            case 31: sort31(out); return;
            case 32: sort32(out); return;
            case 33: sort33(out); return;
            case 34: sort34(out); return;
            case 35: sort35(out); return;
            case 36: sort36(out); return;
            case 37: sort37(out); return;
            case 38: sort38(out); return;
            case 39: sort39(out); return;
            case 40: sort40(out); return;
            case 41: sort41(out); return;
            case 42: sort42(out); return;
            case 43: sort43(out); return;
            case 44: sort44(out); return;
            case 45: sort45(out); return;
            case 46: sort46(out); return;
            case 47: sort47(out); return;
            case 48: sort48(out); return;
            case 49: sort49(out); return;
            case 50: sort50(out); return;
            case 51: sort51(out); return;
            case 52: sort52(out); return;
            case 53: sort53(out); return;
            case 54: sort54(out); return;
            case 55: sort55(out); return;
            case 56: sort56(out); return;
            case 57: sort57(out); return;
            case 58: sort58(out); return;
            case 59: sort59(out); return;
            case 60: sort60(out); return;
            case 61: sort61(out); return;
            case 62: sort62(out); return;
            case 63: sort63(out); return;
            case 64: sort64(out); return;
        }
    }

    // if (len <= 32){
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
        std::memcpy(dst + left, src + left, len * sizeof(int));
        return;
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