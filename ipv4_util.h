#ifndef IPV4_UTIL_H
#define IPV4_UTIL_H

#include <tins/tins.h>
#include <iostream>
#include <limits>

using Tins::IPv4Address;
using std::cout;
using std::endl;

IPv4Address random(IPv4Address addr){
    uint32_t result(addr);
    uint64_t prime_val = 13340323177LLU;
    for(size_t ctr(0); ctr < 3; ++ctr){
	    result = uint64_t(result) * prime_val + 13;
	    result = ((result << 11) | (result >> (32 -11)));
    }

    return IPv4Address(uint32_t(result));
}
#endif 
