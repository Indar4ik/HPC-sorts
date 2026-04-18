#include <algorithm>
#include <chrono>
#include <print>
#include <random>
#include <vector>
#include "sorts/blockquick_sort.hpp"

// std::sort - 1005 ms
// quick_sort (branchless hoar) - 875 ms
// merge_sort - 770 ms
// blockquick_sort - 310 ms
// radix_sort - 95 ms
int main(){
    constexpr size_t ARR_SIZE = 16777216;
    std::vector<int> arr(ARR_SIZE);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-2147483647, 2147483647);

    std::generate(arr.begin(), arr.end(), [&]() {return dist(gen);});

    const auto start1 = std::chrono::steady_clock::now();
    
    quick_sort(arr);
    
    const auto end1 = std::chrono::steady_clock::now();

    if (!std::is_sorted(arr.begin(), arr.end())){
        std::println("ERROR: Array is NOT sorted!");
        return 1;
    }

    double elapsed1 = std::chrono::duration<double, std::milli>(end1 - start1).count();
    
    std::println("elapsed: {:.5f} ms", elapsed1);

    return 0;
}