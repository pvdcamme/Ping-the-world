/**  Pings the whole IPv4 address space.
  *  This program is a work in progress.
  *
  *
  *  This program should run as root.
  *
  *  Optional Arguments:
  *       ./pinger <net-if> <start- IPv4 address> <num addresses>
  *
  *  Default the program uses localhost.
  *   but with the first parameter you can select
  *   which network interface to use.
  *
  * Basic architecture:
  *   -> Pings are generated from 1 thread.
  *       This thread adds a unique identifier to 
  *       to each ping
  *   -> A seperate thread inspects all IMCP traffic.
  *      This thread is also responsible for making
  *      the match between seed & receive.
  * 
  *  This approach has the advantage that we don't need
  *  to cross thread boundaries. The main disadvantage is
  *  that we place all the work in a single thread..
  *   We assume that the network layer is the bottleneck.
  *   
  */
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>

#include <unistd.h>
#include <tins/tins.h>

#include "ipv4_util.h"
#include "time_util.h"
#include "ping_matcher.h"

using std::cout;
using std::endl;

using namespace Tins;

const double PING_RATE = 10; // a sec

/**  Catches all ICMP packts.
  *  This class is intended to process the actual packets.
  */
class ICMPCatcher
{
    const size_t MAX_PACKET_SIZE = 2048;
    const bool PROMISCUOUS = true;
    const std::string netDevice;
    PingMatcher pingMatch;

    Sniffer sniffer;
    std::thread thread_;

    /** Sends a dummy ping.
      * Unfortunately useful to stop ICMPCatcher.
      * Libpcap+libTins are blocking on new packets.
      */
    void sendDummyPing(){
        PacketSender sender(netDevice);
        IP pkt = IP("127.0.0.1") / ICMP(ICMP::ECHO_REQUEST) / RawPDU("foo");
        sender.send(pkt);

    }
public:
    ICMPCatcher(const std::string& device):
        netDevice(device),
        sniffer(device)
    {
        this->sniffer.set_filter("icmp");
        this->sniffer.set_timeout(1);
        thread_ = std::thread( [&]() {
            auto member_callback = std::bind(&ICMPCatcher::handle, this, std::placeholders::_1);
            this->sniffer.sniff_loop(member_callback);
        });
    }

    ICMPCatcher(const ICMPCatcher& oth) = delete;

    ~ICMPCatcher() {
        sniffer.stop_sniff();
        sendDummyPing();
        thread_.join();
    }
    std::vector<PingMatcher::Reply> replies(){
        return pingMatch.replies();
    }

    bool handle(const Packet& pkt) {
        const PDU* pdu = pkt.pdu();

        const IP& ip= pdu->rfind_pdu<IP>();
        const ICMP& icmp = ip.rfind_pdu<ICMP>();
        const Timestamp when = pkt.timestamp();

        auto type = icmp.type();
        if(ICMP::ECHO_REQUEST == type){
            pingMatch.addRequest(when, ip.dst_addr());
        }else if(ICMP::ECHO_REPLY == type){
            pingMatch.addReply(when, ip.src_addr());
        }else{
            //cout << "Received other ICMP: " << type << endl;
        }

        return true;
    }
};

int main(int argc, char** argv)
{
    std::string str_netif("lo");
    uint64_t address_test_count = 1000000;
    IPv4Address start_address("0.0.0.0");
    if(argc >= 2){
        str_netif = argv[1];
    }

    if(argc >= 3){
        start_address = IPv4Address(argv[2]);
    }

    if(argc >= 4){
        std::stringstream in_val(argv[3]);
        in_val >> address_test_count;
    }

    const size_t sleep_between_ping = size_t(1e6/ PING_RATE);
    const size_t THREADING_WAIT = 10000000;

    const IPv4Address local_addr = NetworkInterface(str_netif).ipv4_address();
    cout << "Sending from: " << local_addr << endl;

    ICMPCatcher pingTracer(str_netif);

    usleep(THREADING_WAIT);

    PacketSender sender(str_netif);
    uint64_t pcks_send = 0;

    for(const auto& target_addr: start_address / 0){
        if(pcks_send == 0){
            cout << "First address: " <<  target_addr << endl;
        }

        IP pkt(random(target_addr), local_addr);
        ICMP icmp(ICMP::ECHO_REQUEST);
        RawPDU data("foo");

        icmp.sequence(1 + pcks_send);
        try{
            IP assembled= pkt / icmp /data;
            sender.send(assembled);
            usleep(sleep_between_ping);
        } catch(socket_write_error& ){
          // no interest yet.
        }

        pcks_send++;
        if(pcks_send == address_test_count){
            cout << "Last address: " <<  target_addr << endl;
            break;
        }
    }        

    usleep(THREADING_WAIT);
    cout << "Total send: " << pcks_send << endl;
    auto replies = pingTracer.replies();
    cout << "Received " << replies.size() << " replies" << endl;
    for(auto g: replies){
        cout << "rx: " << IPv4Address(g.destination) << " : " << g.duration <<  " s" << endl;
    }
    return 0;
}
