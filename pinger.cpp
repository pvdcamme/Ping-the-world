/**  Pings the whole IPv4 address space.
  *  This program is a work in progress.
  */
#include <iostream>
#include <tins/tins.h>

using namespace Tins;
using namespace std;


int main(int argc, char** argv)
{
    PacketSender sender;
    IP pkt = IP("192.168.0.1") / ICMP(ICMP::ECHO_REQUEST) / RawPDU("foo");
    sender.send(pkt);
}
