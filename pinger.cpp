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

    /** Sends a dummy ping.
      * Unfortunately useful to stop ICMPCatcher.
      * Libpcap+libTins are blocking on new packets.
      */
    void sendDummyPing(){
        PacketSender sender;
        IP pkt = IP("192.168.0.1") / ICMP(ICMP::ECHO_REQUEST) / RawPDU("foo");
        sender.send(pkt);

    }
public:
    ICMPCatcher(const std::string& device):
        sniffer(device, MAX_PACKET_SIZE, PROMISCUOUS, "icmp")
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

    bool handle(const Packet& pkt) {
        const PDU* pdu = pkt.pdu();
        const ICMP& icmp = pdu->rfind_pdu<ICMP>();
        const Timestamp when = pkt.timestamp();

        cout << "Got an ICMP packet of type: " << icmp.type() << 
           " at " << when.seconds() << "::" << when.microseconds() << endl;
        return true;
    }
};

int main(int argc, char** argv)
{
    ICMPCatcher pingReplies("wlp2s0");

    PacketSender sender;
    IP pkt = IP("192.168.0.1") / ICMP(ICMP::ECHO_REQUEST) / RawPDU("foo");
    sender.send(pkt);

    usleep(1000000);
    return 0;
}
