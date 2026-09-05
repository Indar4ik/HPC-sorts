#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <span>

namespace radix_detail {

// Разряды 11-11-10 бит: три прохода раскладки вместо четырёх по 8 бит.
inline constexpr size_t BUCKETS     = 2048; // 2^11, корзины первых двух проходов
inline constexpr size_t TOP_BUCKETS = 1024; // 2^10, корзины последнего прохода
inline constexpr size_t PREFETCH    = 16;   // на сколько элементов вперёд смотрим

// Time complexity: O(n)
// Space complexity: n
template <bool Signed, typename T, typename Counter>
inline void sort_impl(T* const __restrict src, const size_t n) noexcept {
    static_assert(sizeof(T) == 4, "разбиение 11-11-10 рассчитано на 32-битный ключ");

    // 1. Гистограммы всех трёх разрядов за один проход по данным.
    //    Счётчики разложены на два независимых набора: иначе соседние элементы
    //    с одинаковым разрядом выстраиваются в цепочку store -> load примерно
    //    по 5 тактов, и на неравномерных данных гистограмма дорожает вдвое.
    alignas(64) std::array<std::array<std::array<Counter, BUCKETS>, 3>, 2> part{};
    size_t i = 0;
    for (; i + 1 < n; i += 2){
        const uint32_t x = static_cast<uint32_t>(src[i]);
        const uint32_t y = static_cast<uint32_t>(src[i + 1]);
        ++part[0][0][x & 0x7FF];         ++part[1][0][y & 0x7FF];
        ++part[0][1][(x >> 11) & 0x7FF]; ++part[1][1][(y >> 11) & 0x7FF];
        ++part[0][2][x >> 22];           ++part[1][2][y >> 22];
    }
    if (i < n){
        const uint32_t x = static_cast<uint32_t>(src[i]);
        ++part[0][0][x & 0x7FF];
        ++part[0][1][(x >> 11) & 0x7FF];
        ++part[0][2][x >> 22];
    }

    alignas(64) std::array<std::array<Counter, BUCKETS>, 3> count;
    for (size_t p = 0; p < 3; ++p){
        for (size_t j = 0; j < BUCKETS; ++j){
            count[p][j] = part[0][p][j] + part[1][p][j];
        }
    }

    // 2. Разряд, одинаковый у всех элементов, не переставляет ничего.
    //    Такой проход пропускаем целиком: для данных из узкого диапазона
    //    это убирает половину работы и больше.
    const uint32_t first = static_cast<uint32_t>(src[0]);
    const std::array<bool, 3> need = {
        count[0][first & 0x7FF]         != n,
        count[1][(first >> 11) & 0x7FF] != n,
        count[2][first >> 22]           != n,
    };
    if (!need[0] && !need[1] && !need[2]) return;

    // 3. Префиксные суммы. У знакового ключа старший бит последнего разряда --
    //    это знак, поэтому корзины обходим начиная с 512-й: так отрицательные
    //    числа попадают в начало массива без отдельного прохода.
    for (size_t p = 0; p < 3; ++p){
        if (!need[p]) continue;
        const size_t size  = (p == 2) ? TOP_BUCKETS : BUCKETS;
        const size_t start = (Signed && p == 2) ? TOP_BUCKETS / 2 : 0;
        Counter sum = 0;
        for (size_t s = 0; s < size; ++s){
            const size_t j = (s + start) & (size - 1);
            const Counter c = count[p][j];
            count[p][j] = sum;
            sum += c;
        }
    }

    // 4. Раскладка. make_unique_for_overwrite не зануляет буфер, в отличие от
    //    std::vector, -- на 64 МБ это лишний memset.
    const auto buffer = std::make_unique_for_overwrite<T[]>(n);
    T* from = src;
    T* to   = buffer.get();

    for (size_t p = 0; p < 3; ++p){
        if (!need[p]) continue;
        const int shift = 11 * static_cast<int>(p);
        const uint32_t mask = (p == 2) ? 0x3FF : 0x7FF;
        Counter* const __restrict pos = count[p].data();
        const T* const __restrict in  = from;
        T* const __restrict out = to;

        // Запись разбегается по 2048 адресам, аппаратный префетчер такой поток
        // не распознаёт и каждая запись ждёт свою линию. Считаем разряд
        // элемента, который будем класть через PREFETCH итераций, и тянем
        // целевую линию заранее. Это самая крупная из здешних оптимизаций.
        const size_t lim = (n > PREFETCH) ? n - PREFETCH : 0;
        for (size_t j = 0; j < lim; ++j){
            const uint32_t ahead = (static_cast<uint32_t>(in[j + PREFETCH]) >> shift) & mask;
            __builtin_prefetch(out + pos[ahead], 1, 0);
            const T v = in[j];
            out[pos[(static_cast<uint32_t>(v) >> shift) & mask]++] = v;
        }
        for (size_t j = lim; j < n; ++j){
            const T v = in[j];
            out[pos[(static_cast<uint32_t>(v) >> shift) & mask]++] = v;
        }
        std::swap(from, to);
    }

    // Нечётное число проходов оставляет результат в буфере
    if (from != src) std::memcpy(src, from, n * sizeof(T));
}

// 32-битных счётчиков хватает почти всегда, а 64-битные удваивают таблицы
// и стоят около 12% времени, поэтому широкий вариант -- только для огромных n.
template <bool Signed, typename T>
inline void dispatch(std::span<T> arr) noexcept {
    const size_t n = arr.size();
    if (n <= 1) return;
    if (n <= 0xFFFFFFFFull) sort_impl<Signed, T, uint32_t>(arr.data(), n);
    else                    sort_impl<Signed, T, size_t>(arr.data(), n);
}

} // namespace radix_detail

// На массивах примерно до 2000 элементов постоянные расходы на таблицы
// перевешивают выигрыш -- там быстрее сортировка сравнениями.
inline void radix_sort(std::span<uint32_t> arr) noexcept {
    radix_detail::dispatch<false>(arr);
}

inline void radix_sort(std::span<int32_t> arr) noexcept {
    radix_detail::dispatch<true>(arr);
}
