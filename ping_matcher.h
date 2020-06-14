#ifndef PING_MATCHER_H
#define PING_MATCHER_H
#include <vector>
#include <map>
#include <list>

#include <unistd.h>
#include <tins/tins.h>

#include "time_util.h"

using std::cout;
using std::endl;

using namespace Tins;

/** Matches ping replies to Ping Requests.
  * The main goal is to:
  *   -> From which address did we receive replies?
  *   -> How long did it take to receive the reply?
  *
  *  TODO WIP. Very simple at the moment.
  */ 
class PingMatcher{
public:
    struct Reply{
        double duration;
        uint32_t destination;
    };


private:
    struct Ping{
        Timestamp start;
        uint32_t destination;
    };

    std::list<Ping> requests;
    std::vector<Reply> matches;
    std::map<uint32_t,Ping> address_to_request;

    uint64_t timeout_us_;

    void cleanup(const Timestamp& now){
        auto first_good = requests.begin();
        while(first_good != requests.end() && 
              time_diff(now, first_good->start) >= timeout_us_ * 1e-6){
            address_to_request.erase(first_good->destination);
            first_good++;
        }
        requests.erase(requests.begin(), first_good);
    }

public:
    PingMatcher(uint64_t timeout_us = 5000000):
        timeout_us_(timeout_us)
    { }

   void addRequest(const Timestamp& now, const uint32_t dest){
       cleanup(now);
       Ping req;
       req.start = now;
       req.destination = dest;
       requests.push_back(req);
       address_to_request[dest] = req;
   }

   void addReply(const Timestamp& now, const uint32_t dest){
       cleanup(now);
       auto address_search = address_to_request.find(dest);
       if(address_search != address_to_request.end()){
           Reply reply;
           reply.destination = dest;
           reply.duration = time_diff(now, address_search->second.start);
           matches.push_back(reply);
       }
   }

   std::vector<Reply> replies(){
       return matches;
   }
};

#endif
