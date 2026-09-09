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
// счётчиков стоит около 2.6 мкс независимо от размера массива. Замеры на
// случайных int (мкс): на 256 элементах вставки 2.0 против 3.2 у radix,
// на 512 -- уже 6.0 против 3.8, дальше квадратичный рост берёт своё.
inline constexpr size_t INSERTION_LIMIT = 256;

// Универсальная нестабильная.
// Блочное разбиение без ветвлений, сортировочные сети на хвостах.
inline void sort(std::span<int> arr) noexcept{
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
    if (arr.size() < INSERTION_LIMIT) insertion_sort(arr);
    else radix_sort(arr);
}

inline void integer_sort(std::span<uint32_t> arr) noexcept{
    if (arr.size() < INSERTION_LIMIT) insertion_sort(arr);
    else radix_sort(arr);
}

} // namespace my