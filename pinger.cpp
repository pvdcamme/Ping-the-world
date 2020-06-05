#include <iostream>
#include <tins/tins.h>
#include <string>
#include <tuple>
#include <vector>
#include <algorithm>

using namespace Tins;
using namespace std;


int main(int argc, char** argv)
{
    PacketSender sender;
    IP pkt = IP("192.168.0.1") / ICMP(ICMP::ECHO_REQUEST) / RawPDU("foo");
    sender.send(pkt);
}
