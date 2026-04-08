#include <vector>
#include <print>
#include <random>
#include "sorting_network.hpp"

int main(){
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_int_distribution<> dist(0, 16);
    // std::random_device rdv;
    // std::mt19937 genv(rdv());
    // std::uniform_int_distribution<> distv(-2147483647, 2147483647);
    
    // size_t len = dist(gen);
    // std::vector<int> arr(len);
    // std::generate(arr.begin(), arr.end(), [&]() {return distv(genv);});
    // int* data = arr.data();

    // switch(len){
    //         case 0: case 1: break;
    //         case 2: cmp(data[0], data[1]); break;
    //         case 3: sort3(data); break;
    //         case 4: sort4(data); break;
    //         case 5: sort5(data); break;
    //         case 6: sort6(data); break;
    //         case 7: sort7(data); break;
    //         case 8: sort8(data); break;
    //         case 9: sort9(data); break;
    //         case 10: sort10(data); break;
    //         case 11: sort11(data); break;
    //         case 12: sort12(data); break;
    //         case 13: sort13(data); break;
    //         case 14: sort14(data); break;
    //         case 15: sort15(data); break;
    //         case 16: sort16(data);
    // }

    // bool sorted = std::is_sorted(arr.begin(), arr.end());
    // std::println("{}: {}", len, sorted);

    std::random_device rd;
    std::uniform_int_distribution<> dist(-2147483647, 2147483647);
    std::mt19937 gen(rd());

    for (size_t i = 0; i <= 64; i++){
        std::vector<int> arr(i);
        std::generate(arr.begin(), arr.end(), [&]() {return dist(gen);});
        int* data = arr.data();
        switch(i){
            case 0: case 1: break;
            case 2: cmp(data[0], data[1]); break;
            case 3: sort3(data); break;
            case 4: sort4(data); break;
            case 5: sort5(data); break;
            case 6: sort6(data); break;
            case 7: sort7(data); break;
            case 8: sort8(data); break;
            case 9: sort9(data); break;
            case 10: sort10(data); break;
            case 11: sort11(data); break;
            case 12: sort12(data); break;
            case 13: sort13(data); break;
            case 14: sort14(data); break;
            case 15: sort15(data); break;
            case 16: sort16(data); break;
            case 17: sort17(data); break;
            case 18: sort18(data); break;
            case 19: sort19(data); break;
            case 20: sort20(data); break;
            case 21: sort21(data); break;
            case 22: sort22(data); break;
            case 23: sort23(data); break;
            case 24: sort24(data); break;
            case 25: sort25(data); break;
            case 26: sort26(data); break;
            case 27: sort27(data); break;
            case 28: sort28(data); break;
            case 29: sort29(data); break;
            case 30: sort30(data); break;
            case 31: sort31(data); break;
            case 32: sort32(data); break;
            case 33: sort33(data); break;
            case 34: sort34(data); break;
            case 35: sort35(data); break;
            case 36: sort36(data); break;
            case 37: sort37(data); break;
            case 38: sort38(data); break;
            case 39: sort39(data); break;
            case 40: sort40(data); break;
            case 41: sort41(data); break;
            case 42: sort42(data); break;
            case 43: sort43(data); break;
            case 44: sort44(data); break;
            case 45: sort45(data); break;
            case 46: sort46(data); break;
            case 47: sort47(data); break;
            case 48: sort48(data); break;
            case 49: sort49(data); break;
            case 50: sort50(data); break;
            case 51: sort51(data); break;
            case 52: sort52(data); break;
            case 53: sort53(data); break;
            case 54: sort54(data); break;
            case 55: sort55(data); break;
            case 56: sort56(data); break;
            case 57: sort57(data); break;
            case 58: sort58(data); break;
            case 59: sort59(data); break;
            case 60: sort60(data); break;
            case 61: sort61(data); break;
            case 62: sort62(data); break;
            case 63: sort63(data); break;
            case 64: sort64(data); break;
        }
        bool sorted = std::is_sorted(arr.begin(), arr.end());
        std::println("{}: {}", i, sorted);
    }
    
    return 0;
}