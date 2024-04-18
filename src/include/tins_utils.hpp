#ifndef TINSUTILS__h
#define TINSUTILS__h
// #include <uuid.h>
// #include <boost/uuid/uuid.hpp>
// #include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
// #include <boost/uuid/uuid_generators.hpp> // generators
#include <sstream>
#include <mutex>

#include "hardcoded.hpp"

/* TU - TinsUtils */
namespace TU
{

using namespace Tins;

typedef struct {
    std::string name;
    std::string ip;
    std::string mac;
    bool up;
} BasicInfo_t;

typedef struct {
    int key;
    bool permit;
    std::string ip_addr;
    std::string ip_dst;
    std::string hw_src;
    std::string hw_dst;
    std::string protocol;
    std::string interface;
    std::string direction;
    int port;
} PcapFilter_t;

typedef std::map<std::size_t, std::string> ConHashMap_t;
typedef std::map<std::size_t, int> ColorConMap_t;

#define MAX_COLOR_DEFS 8

class ConnPairs {
private:
    ConHashMap_t _cm;
    ColorConMap_t _colmp;
    uint8_t taken_colors[MAX_COLOR_DEFS];

public:
    ConnPairs() { memset(&taken_colors, 0, sizeof(taken_colors)); }
    ~ConnPairs() { }
    int push(std::size_t hash, char * info);
    int pop(std::size_t hash, char * info);
};

int ConnPairs::push(std::size_t hash, char * info)
{
    if (_cm.count(hash)>0) { // hash is in map
        if ( _colmp.count(hash) > 0) {
            return _colmp[hash];
        }
    } else { // hash is not in map
        std::string x = info;
        _cm[hash] = x;
        for (int i=0; i<MAX_COLOR_DEFS; i++) {
            if (! taken_colors[i]) {
                _colmp[hash] = i;
                taken_colors[i] = 1;
                return i;
            }
        }
    }
    return 0;
}

int ConnPairs::pop(std::size_t hash, char * info)
{
    int ret = 0;
    if (_cm.count(hash)>0) { // hash is in map
        ret = _colmp[hash];
        taken_colors[ret] = 0; // mark as free again
        _cm.erase( hash );
        _colmp.erase( hash );
    } else {
        printf("\n\nerror\n\n");
    }
    return ret; // return the old color so it prints matching close notice
}

class IfaceInfo {
public:
    IfaceInfo() {
        std::cout << "Populating if info.." << std::endl;
        for (const NetworkInterface& iface : NetworkInterface::all()) {
            std::string name = iface.name();
            bool updown = iface.is_up();
            NetworkInterface::Info info = iface.info();
            // BasicInfo_t {name, ip, mac, up}
            _cur.push_back( {name, info.ip_addr.to_string(), info.hw_addr.to_string(), updown} );
        }
    }
    ~IfaceInfo() {}

    bool valid_name(std::string check_name) {
        for (auto const& it: _cur) {
            if (it.name.compare(check_name) == 0) return true;
        }
        return false;
    }

    void dump_shortlist(void) {
        for (auto const& it: _cur) {
            c_paintf_bri_ylw("%15s", it.name.c_str() );
            printf(" <"); c_paintf_wht("%s", it.mac.c_str() ); printf(">  ");
            if (it.up) { c_paintf_bri_grn("up"); }
            else { c_paintf_bri_red("down"); }
            printf("\n");
        }
        printf("\n\n");
    }

private:
    std::vector<BasicInfo_t> _cur;
};

inline void dump_if_list(void) {
    for (const NetworkInterface& iface : NetworkInterface::all()) {
        std::string name = iface.name();
        std::string status = iface.is_up() ? "up" : "down";
        NetworkInterface::Info info = iface.info();
        std::cout << name << ": " << std::endl;

        std::string ipv6_string;
        if (info.ipv6_addrs.empty()) {
            ipv6_string = "(none)";
        } else {
            std::ostringstream oss;
            for (size_t i = 0; i < info.ipv6_addrs.size(); ++i) {
                const NetworkInterface::IPv6Prefix& prefix = info.ipv6_addrs[i];
                if (i > 0) {
                    oss << ", ";
                }
                oss << prefix.address << "/" << prefix.prefix_length;
            }
            ipv6_string = oss.str();
        }
        std::cout   << "   HW address:     " << info.hw_addr << std::endl
                    << "   IP address:     " << info.ip_addr <<  std::endl
                    << "   IPv6 addresses: " << ipv6_string <<  std::endl
                    << "   Netmask:        " << info.netmask <<  std::endl
                    << "   Broadcast:      " << info.bcast_addr <<  std::endl
                    << "   Iface index:    " << iface.id() <<  std::endl
                    << "   Status:         interface " << status << "\n\n";
    }
    // auto *hw = new HWAddress<6>("s5k6sryk");
    // if (hw) std::cout << "success\n";
}

// TODO: imeplement user-defined mappings
inline const char * ip_hex_to_name(uint32_t iphex) {
    // printf("swapped: 0x%04X\n", __builtin_bswap32(iphex));
    switch ( __builtin_bswap32(iphex) ) {
        /* Misc */
        case LOCALHOST_HEX: return "localhost";
        case LO_HEX:        return "lo";
    }
    return "N/A";
}

inline std::string dump_pdu_type(PDU::PDUType type) {
    switch (type) {
        case PDU::ETHERNET_II: return "Ethernet_II";
        case PDU::DOT3:        return "IEEE_802.3";
        case PDU::DOT11:       return "IEEE_802.11";
        case PDU::TCP:         return "TCP";
        case PDU::IP:          return "IPv4";
        case PDU::ARP:         return "ARP";
        case PDU::ICMP:        return "ICMP";
        case PDU::DNS:         return "DNS";
        case PDU::UDP:         return "UDP";
        case PDU::DHCP:        return "DHCP";
        case PDU::LOOPBACK:    return "LOOPBACK";
        case PDU::SNAP:        return "SNAP";
        case PDU::LLC:         return "LLC";
        case PDU::IPv6:        return "IPv6";
        case PDU::STP:         return "STP";
    }
    return "PDU??? - " + std::to_string(type);
}

/* Convert the server endpoint to a readable string */
std::string server_GetStreamIP(const Tins::TCPIP::Stream& stream) {
    std::ostringstream output;
    output << stream.server_addr_v4(); output << ":" << stream.server_port();
    return output.str();
}

/* Convert the client endpoint to a readable string */
std::string client_GetStreamIP(const Tins::TCPIP::Stream& stream) {
    std::ostringstream output;
    output << stream.client_addr_v4(); output << ":" << stream.client_port();
    return output.str();
}

/* Concat both endpoints to get a readable stream identifier */
std::string getStreamIDStr(const Tins::TCPIP::Stream& stream) {
    std::ostringstream output; output << client_GetStreamIP(stream) << " - " << server_GetStreamIP(stream);
    return output.str();
}



} /* TU */


#endif /* TINSUTILS__h */
