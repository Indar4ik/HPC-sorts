#pragma once
#include <cstdint>
#include <span>
#include "counting_sort.hpp"
#include "insertion_sort.hpp"
#include "merge_sort.hpp"
#include "blockquick_sort.hpp"
#include "radix_sort.hpp"

// Единая точка входа. Сами алгоритмы лежат в заголовках рядом и остаются
// доступны напрямую; здесь только выбор подходящего под задачу.
namespace my {

// Ниже этой границы поразрядная сортировка не окупается: обнуление таблиц
// счётчиков стоит около 1.6 мкс независимо от размера массива.
// Замеры на случайных int32, независимые блоки, clang + libc++ (мкс):
//   n:           128     256     512    1024
//   blockquick  0.66    1.53    3.70    8.25
//   radix       1.81    2.16    2.73    3.97
// blockquick на n <= 64 сам уходит в сортировочные сети, поэтому отдельная
// ветка под вставки не нужна -- он быстрее их на всём диапазоне.
inline constexpr size_t BLOCKQUICK_LIMIT = 400;

// Универсальная нестабильная, для любого сравнимого типа.
// Блочное разбиение без ветвлений, сортировочные сети на хвостах.
template <std::ranges::contiguous_range R>
inline void sort(R&& arr) noexcept{
    blockquick_sort(arr);
}

// Универсальная стабильная.
// Слияние по схеме ping-pong, требует n элементов дополнительной памяти.
inline void stable_sort(std::span<int> arr) noexcept{
    merge_sort(arr);
}

// Для 32-битных целых -- самая быстрая из имеющихся.
// Три прохода по 11-11-10 бит, сравнений не делает вовсе.
inline void integer_sort(std::span<int32_t> arr) noexcept{
    if (arr.size() < BLOCKQUICK_LIMIT) blockquick_sort(arr);
    else radix_sort(arr);
}

inline void integer_sort(std::span<uint32_t> arr) noexcept{
    if (arr.size() < BLOCKQUICK_LIMIT) blockquick_sort(arr);
    else radix_sort(arr);
}

} // namespace my