#pragma once
#include <algorithm>
#include <array>
#include <ranges>
#include <cstddef>
#include <cstdint>
#include "sortnet/sorting_network.hpp"

namespace blockquick_detail {

// Размер блока в ЭЛЕМЕНТАХ, не в байтах: он ограничивает карманы смещений и
// длину хвоста, а не объём читаемых данных, поэтому от sizeof(T) не зависит.
// Подобран замером на i5-13400f / clang: одно и то же значение оказалось
// лучшим и для int32, и для int64, и для double. Кривая пологая на 40..64 и
// резко портится после 64 (на отсортированных данных 56 против 72 -- в
// полтора раза).
inline constexpr size_t BLOCK_SIZE = 56;

// Смещения внутри блока помещаются в один байт, пока блок не длиннее 256
static_assert(BLOCK_SIZE <= 256, "смещения хранятся в uint8_t");

// Ниже этой длины разбивать уже невыгодно -- отдаём сортировочной сети
inline constexpr size_t NETWORK_LIMIT = 64;

// Блочное разбиение Эдельcбруннера-Вайса: сравнения и перестановки разнесены
// по разным циклам, поэтому в горячем цикле нет ни одного непредсказуемого
// ветвления -- номера «нарушителей» просто накапливаются в карманах.
template <typename T>
T* block_partition(T* __restrict first, T* __restrict last) noexcept {
    const size_t len = static_cast<size_t>(last - first);

    // 1. Выбор пивота (медиана из трёх) + перестановка трёх элементов в правильном порядке
    T* mid = first + len / 2;
    if (*mid < *first) std::swap(*mid, *first);
    if (*(last - 1) < *first) std::swap(*(last - 1), *first);
    if (*(last - 1) < *mid) std::swap(*(last - 1), *mid);
    std::swap(*first, *mid); // Теперь медиана лежит в *first

    const T pivot = *first;
    T* l = first + 1;
    T* r = last; // Полуинтервал

    // Карманы для индексов нарушителей
    std::array<uint8_t, BLOCK_SIZE> offsets_l;
    std::array<uint8_t, BLOCK_SIZE> offsets_r;

    size_t num_l = 0, num_r = 0;     // Сколько нарушителей найдено
    size_t start_l = 0, start_r = 0; // Откуда начинаем брать индексы для свапа

    // 2. Главный блочный цикл
    while (static_cast<size_t>(r - l) >= 2 * BLOCK_SIZE) {
        // Если левый карман пуст, сканируем следующий блок.
        // Индекс пишется всегда, а счётчик двигается только на нарушителе:
        // так вместо перехода получается setcc + add.
        if (num_l == 0) {
            start_l = 0;
            for (size_t i = 0; i < BLOCK_SIZE; ++i) {
                offsets_l[num_l] = static_cast<uint8_t>(i);
                num_l += (l[i] >= pivot);
            }
        }
        // То же самое справа, встречным ходом
        if (num_r == 0) {
            start_r = 0;
            for (size_t i = 0; i < BLOCK_SIZE; ++i) {
                offsets_r[num_r] = static_cast<uint8_t>(i);
                num_r += (r[-1 - static_cast<ptrdiff_t>(i)] <= pivot);
            }
        }

        // 3. Обмениваем элементы по сохранённым индексам
        const size_t swaps = std::min(num_l, num_r);
        for (size_t i = 0; i < swaps; ++i) {
            std::swap(l[offsets_l[start_l + i]],
                      r[-1 - static_cast<ptrdiff_t>(offsets_r[start_r + i])]);
        }

        // Обновляем счётчики карманов
        num_l -= swaps; start_l += swaps;
        num_r -= swaps; start_r += swaps;

        // Если карман полностью опустел, сдвигаем указатель блока
        if (num_l == 0) l += BLOCK_SIZE;
        if (num_r == 0) r -= BLOCK_SIZE;
    }

    // 4. Остаток: меньше 2*BLOCK_SIZE элементов в [l, r).
    // Прогоняем по ним branchless-Ломуто
    for (T* curr = l; curr < r; ++curr) {
        const T val = *curr;
        const T tmp = *l;
        const bool is_less = (val < pivot);

        *curr = is_less ? tmp : val;
        *l = is_less ? val : tmp;

        l += is_less;
    }

    // 5. Ставим пивот на его законное место
    T* pivot_pos = l - 1;
    std::swap(*first, *pivot_pos);
    return pivot_pos;
}

// Рекурсия идёт по указателям: span здесь пришлось бы собирать заново на
// каждом уровне, а пользы от него внутри нет
template <typename T>
void sort_range(T* first, T* last) noexcept {
    while (static_cast<size_t>(last - first) > NETWORK_LIMIT) {
        T* p = block_partition(first, last);

        // Левая часть пуста -- значит пивот оказался минимумом отрезка.
        // Справа всё >= пивота, поэтому равные ему можно просто пропустить,
        // не разбивая дальше. Без этого массив из повторяющихся значений
        // резал бы по одному элементу за проход, то есть за O(n^2):
        // на 4M элементов с двумя тысячами различных значений
        // это разница между 380 мс и 47 мс.
        if (p == first) {
            const T pivot = *p;
            T* q = p + 1;
            while (q < last && !(pivot < *q)) ++q;
            first = q;
            continue;
        }

        // Оптимизация хвостовой рекурсии (защита стека): рекурсия всегда
        // в меньшую половину, большая доедается этим же циклом
        if (p - first < last - (p + 1)) {
            sort_range(first, p);
            first = p + 1;
        } else {
            sort_range(p + 1, last);
            last = p;
        }
    }

    network_sort(first, static_cast<size_t>(last - first));
}

} // namespace blockquick_detail

// Time complexity:
// Best: O(n log n)
// Average: O(n log n)
// Worst: O(n^2)
// Space complexity: O(log n) на стек рекурсии
//
// Принимает любой непрерывный диапазон -- vector, array, span, C-массив.
// Через параметр std::span<T> тип не выводился бы: из vector<int> компилятор
// сам span не соберёт, и вызов blockquick_sort(v) не собрался бы.
template <std::ranges::contiguous_range R>
inline void blockquick_sort(R&& arr) noexcept {
    auto* const first = std::ranges::data(arr);
    blockquick_detail::sort_range(first, first + std::ranges::size(arr));
}
