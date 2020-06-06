/**  Pings the whole IPv4 address space.
  *  This program is a work in progress.
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
public:
    ICMPCatcher():
        sniffer("wlp2s0", MAX_PACKET_SIZE, PROMISCUOUS)
    {
        this->sniffer.set_timeout(1);
        thread_ = std::thread( [&]() {
            auto member_callback = std::bind(&ICMPCatcher::handle, this, std::placeholders::_1);
            this->sniffer.sniff_loop(member_callback);
        });
    }

    ~ICMPCatcher() {
        sniffer.stop_sniff();
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
    ICMPCatcher cc;

    PacketSender sender;
    IP pkt = IP("192.168.0.1") / ICMP(ICMP::ECHO_REQUEST) / RawPDU("foo");
    sender.send(pkt);

    usleep(1000000);
    return 0;
}
