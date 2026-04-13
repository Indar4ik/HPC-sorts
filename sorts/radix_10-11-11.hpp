#pragma once
#include <span>
#include <vector>
#include <cstdint>
#include <cstring>

// Time complexity: T(n) = 6n
// Space complexity: S(n) = 2n
void radix_sort(std::span<int> arr) noexcept{
    const size_t n = arr.size();
    if (n <= 1) return;
    
    std::vector<int> buffer(n);
    int* const src = arr.data();
    int* const dst = buffer.data();

    size_t count[3][2048] = {0};
    
    for (size_t i = 0; i < n; ++i){
        uint32_t v = static_cast<uint32_t>(src[i]);
        count[0][v & 0x7FF]++;
        count[1][(v >> 11) & 0x7FF]++;
        count[2][(v >> 22) & 0x3FF]++;
    }

    for (size_t i = 0; i < 2; ++i){
        size_t sum = 0;
        for (size_t j = 0; j < 2048; ++j){
            size_t v = count[i][j];
            count[i][j] = sum;
            sum += v;
        }
    }

    size_t sum = 0;
    for (size_t j = 512; j < 1024; ++j){
        size_t v = count[2][j];
        count[2][j] = sum;
        sum += v;
    }
    for (size_t j = 0; j < 512; ++j){
        size_t v = count[2][j];
        count[2][j] = sum;
        sum += v;
    }

    for (size_t i = 0; i < n; ++i){
        uint32_t v = static_cast<uint32_t>(src[i]);
        dst[count[0][v & 0x7FF]++] = src[i];
    }
    for (size_t i = 0; i < n; ++i){
        uint32_t v = static_cast<uint32_t>(dst[i]);
        src[count[1][(v >> 11) & 0x7FF]++] = dst[i];
    }
    for (size_t i = 0; i < n; ++i){
        uint32_t v = static_cast<uint32_t>(src[i]);
        dst[count[2][(v >> 22) & 0x3FF]++] = src[i];
    }
    std::memcpy(src, dst, n * sizeof(int));
}