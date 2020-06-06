/** This class ads some tests for the pinger code.
  * We focus mostuly on testing the utilities around 
  *  the pinger.
  */ 
#include <cassert>
#include <iostream>
#include <unistd.h>

#include <tins/tins.h>

#include "time_util.h"

using namespace Tins;

bool about_same(double a, double b){
    double diff = a - b;
    double delta = 1e-9;
    return -delta < diff && diff < delta;
}


void test_time_diff_reversed(){
    Timestamp old = Timestamp::current_time();
    usleep(100);

    Timestamp young= Timestamp::current_time();
    assert(time_diff(old,young) < 0);
}

void test_time_diff_normal_case_with_wrap(){
    timeval tv_older;
    tv_older.tv_sec= 32124;
    tv_older.tv_usec = 999999 ;

    timeval tv_younger;
    tv_younger.tv_sec= 32125;
    tv_younger.tv_usec = 1;

    double calc_diff = time_diff(Timestamp(tv_younger), Timestamp(tv_older));
    assert(about_same(calc_diff, 2e-6));
}

void test_time_diff_normal_case_no_wrap(){
    double extra = 300;
    double micosec = 1e-6;
    timeval tv_older;
    tv_older.tv_sec= 31245;
    tv_older.tv_usec = 321;

    timeval tv_younger;
    tv_younger.tv_sec= 31245;
    tv_younger.tv_usec = 321 + extra;

    double calc_diff = time_diff(Timestamp(tv_younger), Timestamp(tv_older));
    assert(about_same(calc_diff, extra * micosec));
}

void test_time_diff_same_is_zero(){
    Timestamp a_stamp= Timestamp::current_time();
    
    double calc_diff = time_diff(a_stamp, a_stamp);
    assert(about_same(calc_diff, 0));
}
int main(){
    test_time_diff_same_is_zero();
    test_time_diff_normal_case_no_wrap();
    test_time_diff_normal_case_with_wrap();
    test_time_diff_reversed();

    std::cout << "All tests pass " << std::endl;
    return 0;
}
