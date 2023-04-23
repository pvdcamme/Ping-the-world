/** This class ads some tests for the pinger code.
  * We focus mostuly on testing the utilities around 
  *  the pinger.
  */ 
#include <cassert>
#include <iostream>
#include <unistd.h>
#include <bitset>
#include <vector>
#include <limits>
#include <algorithm>


#include <tins/tins.h>

#include "time_util.h"
#include "ipv4_util.h"
#include "ping_matcher.h"

using namespace Tins;

/* Test utility function, are the floating point values nearly the same.
 */
bool about_same(double a, double b){
    double diff = a - b;
    double delta = 1e-9;
    return -delta < diff && diff < delta;
}

/* Test utility to make creating timestamps easier
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

void test_ping_matcher_normal_single_ping(){
    PingMatcher p;
    uint32_t addr = IPv4Address("127.0.0.1");

    p.addRequest(Timestamp::current_time(), addr);
    p.addReply(Timestamp::current_time(), addr);

    auto replies = p.replies();
    assert(replies.size() == 1);
    assert(replies[0].destination == addr);
}

void test_ping_matcher_no_response_single_ping(){
    PingMatcher p;
    uint32_t addr = IPv4Address("127.0.0.1");

    p.addRequest(Timestamp::current_time(), addr);

    auto replies = p.replies();
    assert(replies.size() == 0);
}

void test_ping_matcher_timeout_single_ping(){
    PingMatcher p(1000000);
    uint32_t addr = IPv4Address("127.0.0.1");
    Timestamp request_time = create_timestamp(1235,124);
    Timestamp reply_time = create_timestamp(1236,424);

    p.addRequest(request_time, addr);
    p.addReply(reply_time, addr);

    auto replies = p.replies();
    assert(replies.size() == 0);
}

void test_ping_matcher_reply_without_request(){
    PingMatcher p(1000000);
    uint32_t addr = IPv4Address("127.0.0.1");

    p.addReply(Timestamp::current_time(), addr);

    auto replies = p.replies();
    assert(replies.size() == 0);
}


void test_ping_matcher_multiple_normal(){
    PingMatcher p(1000000);
    auto addr_range = IPv4Address("127.0.0.1") /12;
    size_t n_replies = 0;

    auto start = Timestamp::current_time();

    for(auto local: addr_range){
        p.addRequest(start, local);
    }

    auto response = Timestamp::current_time();

    for(auto local: addr_range){
        p.addReply(response, local);
        n_replies++;
    }

    auto replies = p.replies();
    cout << "Received " << replies.size() << " -- expected: " << n_replies << endl;
    assert(replies.size() == n_replies);
}

void test_ipv4_full_range(){
    const uint64_t TOTAL_RANGE = std::numeric_limits<uint32_t>::max();
    std::vector<bool>seen_addr(TOTAL_RANGE);


    for(uint64_t local(0); local <= TOTAL_RANGE ; ++local){
        auto new_addr = random(IPv4Address(local));
        uint32_t new_spot(new_addr);

        assert(false == seen_addr[new_spot]);
        seen_addr[new_spot] = true;
    }

    assert(std::all_of(seen_addr.begin(), seen_addr.end(), [](bool v) { return true == v; }));
}

int main(){
    test_time_diff_same_is_zero();
    test_time_diff_normal_case_no_wrap();
    test_time_diff_normal_case_with_wrap();
    test_time_diff_reversed();
    
    test_ping_matcher_normal_single_ping();
    test_ping_matcher_no_response_single_ping();
    test_ping_matcher_timeout_single_ping();
    test_ping_matcher_reply_without_request();
    test_ping_matcher_multiple_normal();

    test_ipv4_full_range();

    std::cout << "All tests pass " << std::endl;
    return 0;
}
