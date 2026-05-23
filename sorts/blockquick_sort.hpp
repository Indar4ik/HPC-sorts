#pragma once
#include <algorithm>
#include <span>
#include <cstdint>
#include "sortnet/sorting_network.hpp"

// Размер блока. 64 элемента = 256 байт (идеально для L1 и регистров)
inline constexpr size_t BLOCK_SIZE = 64;

[[gnu::hot]]
static int* block_partition(int* __restrict first, int* __restrict last) noexcept {
    size_t len = last - first;
    
    // 1. Выбор Пивота (Медиана из трех) + легкая пре-сортировка
    int* mid = first + len / 2;
    if (*mid < *first) std::swap(*mid, *first);
    if (*(last - 1) < *first) std::swap(*(last - 1), *first);
    if (*(last - 1) < *mid) std::swap(*(last - 1), *mid);
    std::swap(*first, *mid); // Теперь медиана лежит в *first
    
    int pivot = *first;
    int* l = first + 1;
    int* r = last; // Полуинтервал

    // "Карманы" для индексов нарушителей
    uint8_t offsets_l[BLOCK_SIZE];
    uint8_t offsets_r[BLOCK_SIZE];
    
    int num_l = 0, num_r = 0;     // Сколько нарушителей найдено
    int start_l = 0, start_r = 0; // Откуда начинаем брать индексы для свапа

    // 2. Главный Блочный Цикл
    while (r - l >= 2 * BLOCK_SIZE) {
        // Если левый карман пуст, сканируем следующие 64 элемента
        if (num_l == 0) {
            start_l = 0;
            for (uint8_t i = 0; i < BLOCK_SIZE; ++i) {
                offsets_l[num_l] = i;
                num_l += (l[i] >= pivot); // BRANCHLESS! Предсказатель спит.
            }
        }
        // Если правый карман пуст, сканируем следующие 64 элемента
        if (num_r == 0) {
            start_r = 0;
            for (uint8_t i = 0; i < BLOCK_SIZE; ++i) {
                offsets_r[num_r] = i;
                num_r += (r[-1 - i] <= pivot); // BRANCHLESS!
            }
        }

        // 3. Обмениваем элементы по сохраненным индексам
        int swaps = std::min(num_l, num_r);
        for (int i = 0; i < swaps; ++i) {
            std::swap(l[offsets_l[start_l + i]], r[-1 - offsets_r[start_r + i]]);
        }

        // Обновляем счетчики карманов
        num_l -= swaps; start_l += swaps;
        num_r -= swaps; start_r += swaps;

        // Если карман полностью опустел, сдвигаем указатель блока
        if (num_l == 0) l += BLOCK_SIZE;
        if (num_r == 0) r -= BLOCK_SIZE;
    }

    // 4. ОСТАТОК (Tail Processing)
    // У нас осталось меньше 128 элементов[l, r).
    // Мы прогоняем по ним Branchless Ломуто!
    int* left = l;
    for (int* curr = l; curr < r; ++curr) {
        int val = *curr;
        int tmp = *left;
        bool is_less = (val < pivot);
        
        *curr = is_less ? tmp : val; // CMOV
        *left = is_less ? val : tmp; // CMOV
        
        left += is_less; // BRANCHLESS
    }

    // 5. Ставим пивот на его законное место
    int* pivot_pos = left - 1;
    std::swap(*first, *pivot_pos);
    return pivot_pos;
}

inline void blockquick_sort(std::span<int> arr) noexcept {
    int* first = arr.data();
    int* last = first + arr.size();
    
    while (last - first > 64) {
        int* p = block_partition(first, last);
        
        // Оптимизация хвостовой рекурсии (защита стека)
        if (p - first < last - (p + 1)) {
            blockquick_sort(std::span<int>(first, p - first));
            first = p + 1; // Цикл while займется бОльшей правой частью
        } else {
            blockquick_sort(std::span<int>(p + 1, last - (p + 1)));
            last = p;      // Цикл while займется бОльшей левой частью
        }
    }
    
    size_t len = last - first;
    network_sort(first, len);
}