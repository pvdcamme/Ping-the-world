The goal of this project is to ping the whole IPv4 address space.

To this end you'll find the pinger program in this repository. It adds
a couple features to make pinging easier

 - rate limit the number of pings
 - randomization of the IPv4 address space.

 These all together should avoid getting hit by a rate-limit too often.

In addition, this pinger did run on a raspberry Pi, low-power but slightly
unreliable platform, at least how mine is powered. As such the pinger works
in 'small chunks' each time.

