#ifndef TIME_UTIL_H
#define TIME_UTIL_H
#include <tins/tins.h>

using namespace Tins;

double time_diff(const Timestamp& young, const Timestamp& old){
    if(young.seconds() < old.seconds() || 
      (young.seconds() == old.seconds() && young.microseconds() < old.microseconds())){
        return - time_diff(old,young);
    }
    int second_diff = young.seconds() - old.seconds();
    int extra = 0;

    if(young.microseconds() < old.microseconds()){
        second_diff -= 1;
        extra = 1000000;
    }
    int micro_sec_diff = (extra + young.microseconds()) - old.microseconds();
    return second_diff + micro_sec_diff * 1e-6;
}

#endif

