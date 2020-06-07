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

/* Test utility method.
 */
bool about_same(double a, double b){
    double diff = a - b;
    double delta = 1e-9;
    return -delta < diff && diff < delta;
}

/* Test utility to make creating timestamps easier
 * 
 */

Timestamp create_timestamp(uint64_t sec, uint64_t microsec){
    timeval tv;
    tv.tv_sec= sec;
    tv.tv_usec = microsec;
    return Timestamp(tv);
}


void test_time_diff_reversed(){
    Timestamp old = Timestamp::current_time();
    usleep(100);

    Timestamp young= Timestamp::current_time();
    assert(time_diff(old,young) < 0);
}

void test_time_diff_normal_case_with_wrap(){
    Timestamp older = create_timestamp(32124, 999999);
    Timestamp younger  = create_timestamp(32125, 1);

    double calc_diff = time_diff(younger, older);
    assert(about_same(calc_diff, 2e-6));
}

void test_time_diff_normal_case_no_wrap(){
    double extra = 300;
    double micosec = 1e-6;

    Timestamp older = create_timestamp(3135, 3121);
    Timestamp younger= create_timestamp(3135, 3121 + extra);

    double calc_diff = time_diff(younger, older);
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
