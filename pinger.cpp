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

        cout << "Go for thread" << endl;
        thread_ = std::thread( [&]() {
            cout << "Let's start capturing" << endl;
            this->sniffer.sniff_loop(std::bind(&ICMPCatcher::handle, this, std::placeholders::_1));
        });
    }

    ~ICMPCatcher() {
        sniffer.stop_sniff();
        thread_.join();
    }

    bool handle(PDU& p) {
        cout << "Got a pakcet" << endl;
        return false;
    }
};

int main(int argc, char** argv)
{
    PacketSender sender;
    IP pkt = IP("192.168.0.1") / ICMP(ICMP::ECHO_REQUEST) / RawPDU("foo");
    sender.send(pkt);

    ICMPCatcher cc;
    usleep(1000000);
    cout << "Time for sleep" << endl;
}
