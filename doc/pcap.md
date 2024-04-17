# Libpcap


### filters

```sh
# filters
man 7 pcap-filter

# Packets to or from host
host box4
# or by IP
host 8.8.8.8

# Packets between hosts
host box2 and box4

# Packets by source
src 10.2.4.1

# By destination
dst 99.99.2.2

# By port
port 143
portange 1-1024

# By source/destination port
src port 22
dst port 80

# By Protocol
tcp
udp
icmp

# And
src localhost and dst port 22
src localhost && dst port 22

# Or
port 80 or 22
port 80 || 22

# Grouping
src localhost and (dst port 80 or 22 or 443)
```



#### end
