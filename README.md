# promacct: a pcap-based traffic accounting tool for Prometheus

The utility provided by this repository is similar to
[pmacct](http://www.pmacct.net/). It can use
[libpcap](http://www.tcpdump.org/) to inspect traffic on a network
interface and store statistics regarding the number of packets and their
size. In addition to storing the total amount of traffic, it also has
support for aggregating by source/destination IPv4 address.

Where this utility differs from pmacct is that it doesn't store its
results in a database. Instead, it binds a HTTP server that exports a
metrics page that can be scraped by Prometheus. By default, this HTTP
server listens on port 9112.

# Building promacct

Right now promacct is still simple enough that it can easily be built by
hand. Be sure to take a look at `build.sh` to see how. The script
`build_static.sh` builds a statically linked executable for Linux-based
systems.

Promacct has very few dependencies. It's written in C++, making use of
certain C++14 features. It makes use of libpcap.

# Using promacct

After building promacct, it can be started as follows:

```
promacct -i eth0 -i eth1 -r 192.168.1.100-192.168.1.200
```

This makes promacct sniff for traffic on eth0 and eth1, storing the
total amount of traffic in separate histograms. It also creates
histograms for the aggregated amount of network traffic for every
individual IPv4 address between 192.168.1.100 and 192.168.1.200.

# Useful Prometheus rules

The following rule can be used to compute a five-minute rate of all
traffic per host and network interface:

```
instance_interface:promacct_packet_size_bytes_all:rate5m =
    sum(rate(promacct_packet_size_bytes_all_sum{job="promacct"}[5m]))
    by (instance, interface)
```

This metric can be used to compute a monthly 95th percentile as follows:

```
instance_interface:promacct_packet_size_bytes_all:quantile31d{quantile="0.95"} =
    quantile_over_time(0.95, instance_interface:promacct_packet_size_bytes_all:rate5m[31d])
```
