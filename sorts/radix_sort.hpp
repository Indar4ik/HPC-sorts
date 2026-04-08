#pragma once
#include <span>
#include <vector>
#include <cstdint>

// Time complexity: T(n) = 6n
// Space complexity: S(n) = 2n
void radix_sort(std::span<int> arr) noexcept{
    const size_t n = arr.size();
    if (n <= 1) return;
    
    std::vector<int> buffer(n);
    int* const src = arr.data();
    int* const dst = buffer.data();

    size_t count[4][256] = {0};
    
    for (size_t i = 0; i < n; ++i){
        uint32_t v = static_cast<uint32_t>(src[i]);
        count[0][v & 0xFF]++;
        count[1][(v >> 8) & 0xFF]++;
        count[2][(v >> 16) & 0xFF]++;
        count[3][(v >> 24) & 0xFF]++;
    }

    for (size_t i = 0; i < 3; ++i){
        size_t sum = 0;
        for (size_t j = 0; j < 256; ++j){
            size_t v = count[i][j];
            count[i][j] = sum;
            sum += v;
        }
    }

    size_t sum = 0;
    for (size_t j = 128; j < 256; ++j){
        size_t v = count[3][j];
        count[3][j] = sum;
        sum += v;
    }
    for (size_t j = 0; j < 128; ++j){
        size_t v = count[3][j];
        count[3][j] = sum;
        sum += v;
    }

    for (size_t i = 0; i < n; ++i){
        uint32_t v = static_cast<uint32_t>(src[i]);
        dst[count[0][v & 0xFF]++] = src[i];
    }
    for (size_t i = 0; i < n; ++i){
        uint32_t v = static_cast<uint32_t>(dst[i]);
        src[count[1][(v >> 8) & 0xFF]++] = dst[i];
    }
    for (size_t i = 0; i < n; ++i){
        uint32_t v = static_cast<uint32_t>(src[i]);
        dst[count[2][(v >> 16) & 0xFF]++] = src[i];
    }
    for (size_t i = 0; i < n; ++i){
        uint32_t v = static_cast<uint32_t>(dst[i]);
        src[count[3][(v >> 24) & 0xFF]++] = dst[i];
    }
}