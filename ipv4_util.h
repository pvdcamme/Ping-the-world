#ifndef IPV4_UTIL_H
#define IPV4_UTIL_H

#include <tins/tins.h>


IPv4Address random(IPv4Address addr){
    uint32_t base_addr(addr);
    uint64_t prime_val = 9127;
    uint64_t other_prime = 13;
    return IPv4Address(uint64_t(base_addr) * prime_val + other_prime);
}
#endif 
