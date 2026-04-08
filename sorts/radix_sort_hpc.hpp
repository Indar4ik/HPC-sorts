// radix_sort_hpc.hpp
#pragma once
#include <span>
#include <vector>
#include <cstdint>
#include <cstring>
#include <utility>
#include <bit>

#if defined(__clang__) || defined(__GNUC__)
#  define RS_HOT            __attribute__((hot))
#  define RS_ALWAYS_INLINE  __attribute__((always_inline)) inline
#  define RS_PREFETCH_W(p)  __builtin_prefetch((p), 1, 0)
#else
#  define RS_HOT
#  define RS_ALWAYS_INLINE  inline
#  define RS_PREFETCH_W(p)  (void)(p)
#endif

namespace hpc {
namespace detail {

inline constexpr int    PASSES    = 4;
inline constexpr int    BUCKETS   = 256;
inline constexpr size_t PREF_DIST = 48;

using Cnt = uint32_t;

RS_HOT
static void build_histogram(const int* __restrict data,
                             size_t n,
                             Cnt cnt[PASSES][BUCKETS]) noexcept
{
    for (size_t i = 0; i < n; ++i) {
        const uint32_t v = static_cast<uint32_t>(data[i]);
        ++cnt[0][ v        & 0xFFu];
        ++cnt[1][(v >>  8) & 0xFFu];
        ++cnt[2][(v >> 16) & 0xFFu];
        ++cnt[3][(v >> 24) & 0xFFu];
    }
}

RS_HOT
static bool prefix_and_skip(Cnt* __restrict cnt, bool sign_pass) noexcept
{
    Cnt sum    = 0;
    int active = 0;

    if (!sign_pass) {
        for (int j = 0; j < BUCKETS; ++j) {
            active += (cnt[j] != 0u);
            const Cnt c = cnt[j];
            cnt[j] = sum;
            sum   += c;
        }
    } else {
        for (int j = 128; j < BUCKETS; ++j) {
            active += (cnt[j] != 0u);
            const Cnt c = cnt[j];
            cnt[j] = sum;
            sum   += c;
        }
        for (int j = 0; j < 128; ++j) {
            active += (cnt[j] != 0u);
            const Cnt c = cnt[j];
            cnt[j] = sum;
            sum   += c;
        }
    }
    return (active <= 1);
}

RS_HOT RS_ALWAYS_INLINE
static void scatter(const int* __restrict in,
                    int*       __restrict out,
                    size_t n,
                    int shift,
                    Cnt* __restrict pfx) noexcept
{
    for (size_t i = 0; i < n; ++i) {
        if (i + PREF_DIST < n) [[likely]] {
            const uint32_t vf = static_cast<uint32_t>(in[i + PREF_DIST]);
            RS_PREFETCH_W(out + pfx[(vf >> shift) & 0xFFu]);
        }
        const uint32_t v = static_cast<uint32_t>(in[i]);
        const uint32_t b = (v >> shift) & 0xFFu;
        out[pfx[b]++] = std::bit_cast<int>(v);
    }
}

} // namespace detail

RS_HOT
void radix_sort(std::span<int> arr) noexcept
{
    using namespace detail;

    const size_t n = arr.size();
    if (n <= 1) return;

    std::vector<int> buf(n);
    int* const base = arr.data();
    int* const tmp  = buf.data();

    alignas(64) Cnt cnt[PASSES][BUCKETS] {};

    build_histogram(base, n, cnt);

    bool skip[PASSES];
    for (int p = 0; p < PASSES - 1; ++p)
        skip[p] = prefix_and_skip(cnt[p], false);
    skip[PASSES - 1] = prefix_and_skip(cnt[PASSES - 1], true);

    int* in  = base;
    int* out = tmp;
    constexpr int SHIFTS[PASSES] = {0, 8, 16, 24};

    for (int p = 0; p < PASSES; ++p) {
        if (skip[p]) continue;
        scatter(in, out, n, SHIFTS[p], cnt[p]);
        std::swap(in, out);
    }

    if (in != base)
        std::memcpy(base, tmp, n * sizeof(int));
}

} // namespace hpc
