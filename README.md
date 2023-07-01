The goal of this project is to ping the whole IPv4 address space.

To this end you'll find the pinger program in this repository. It adds the following features to make pinging easier
 - To avoid throttling of the local ISP, rate limits the number of pings. 
 - Randomization of the IPv4 address space, to maximize the time the same subnet is revisited.

the pinger requires few resources, it ran on a raspberry Pi. The ISP rate-limit being the major bottleneck (for good reason!).
Since the IPv4 address is still large, run-time is in the months; As such the pinger works in 'small chunks' each time.


Programs
---

- ./pinger <start addrress> <amount of hosts to ping>
- ./test: no arguments runs the test-code. Best run on a slightly faster machine than raspberry pi.

