/**  Pings the whole IPv4 address space.
  *  This program is a work in progress.
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
#include <iostream>
#include <functional>
#include <thread>
#include <string>

#include <unistd.h>
#include <tins/tins.h>

using std::cout;
using std::endl;

using namespace Tins;


/**  Catches all ICMP packts.
  *  This class is intended to process the actual packets.
  */
class ICMPCatcher
{
    const size_t MAX_PACKET_SIZE =2048;
    const bool PROMISCUOUS = true;

    Sniffer sniffer;
    std::thread thread_;
    uint64_t nPingRequest;
    uint64_t nPingReply;


    /** Sends a dummy ping.
      * Unfortunately useful to stop ICMPCatcher.
      * Libpcap+libTins are blocking on new packets.
      */
    void sendDummyPing(){
        PacketSender sender;
        IP pkt = IP("127.0.0.1") / ICMP(ICMP::ECHO_REQUEST) / RawPDU("foo");
        sender.send(pkt);

    }
public:
    ICMPCatcher(const std::string& device):
        sniffer(device, MAX_PACKET_SIZE, PROMISCUOUS, "icmp"),
        nPingRequest(0), nPingReply(0)
    {
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

    uint64_t requests(){
        return nPingRequest;
    }

    uint64_t replies(){
        return nPingReply;
    }

    bool handle(const Packet& pkt) {
        const PDU* pdu = pkt.pdu();
        const ICMP& icmp = pdu->rfind_pdu<ICMP>();
        const RawPDU& raw= icmp.rfind_pdu<RawPDU>();
        //const Timestamp when = pkt.timestamp();
        auto type = icmp.type();
        if(ICMP::ECHO_REQUEST == type){
            nPingRequest++;
        }else if(ICMP::ECHO_REPLY == type){
            nPingReply++;
        }

        auto pp = raw.payload();
        cout << std::string(pp.begin(), pp.end()) << endl;

        return true;
    }
};

int main(int argc, char** argv)
{
    const std::string netif("lo");
    ICMPCatcher pingTracer(netif);

    usleep(100000);

    PacketSender sender(netif);
    IPv4Range range = IPv4Address("127.0.1.0") / 20;

    uint64_t pcks_send = 0;
    for(const auto& local_addr: range){
        IP pkt= IP(local_addr, "127.0.0.1") / ICMP(ICMP::ECHO_REQUEST) / RawPDU("foo");
        try{
            sender.send(pkt);
        } catch(socket_write_error){
            //ignore
        }
        pcks_send++;
    }        
    usleep(1000);
    cout << "Total send: " << pcks_send << endl;
    cout << "requests: " << pingTracer.requests() << endl;
    cout << "replies: " << pingTracer.replies() << endl;
    return 0;
}
