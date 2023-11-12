The goal of this project is to ping the whole IPv4 address space.

To this end you'll find a  pinger program in this repository. To make pinging easier is has the following
 - Rate limiting pings, to avoid throttling by e.g. the local ISP.
 - Randomization of the IPv4 address space, to avoid being hit by rate-limiting on the other end.

the pinger requires few resources, it ran on a raspberry Pi with Tthe ISP rate-limit being the major bottleneck (for good reason!).
Since the IPv4 address is still large, run-time is in the months; As such the pinger works in 'small chunks' each time.


Programs
---

- `./pinger <start addrress> <amount of hosts to ping>`
- `./test: no arguments runs the test-code. Best run on a slightly faster machine than raspberry pi.`

