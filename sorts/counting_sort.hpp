#pragma once
#include <span>
#include <algorithm>
#include <vector>

void counting_sort(std::span<int> arr){
    if (arr.size() <= 1) return;
    constexpr int MIN_VAL = -2048; 
    constexpr int MAX_VAL = 2047;
    constexpr size_t RANGE = MAX_VAL - MIN_VAL + 1;
    size_t count[RANGE] = {0}; 
    int* const data = arr.data();
    const size_t n = arr.size();
    for (size_t i = 0; i < n; ++i) {
        count[data[i] - MIN_VAL]++;
    }
    size_t idx = 0;
    for (size_t i = 0; i < RANGE; ++i) {
        size_t c = count[i];
        if (c > 0) {
            std::fill_n(data + idx, c, i + MIN_VAL);
            idx += c;
        }
    }
}