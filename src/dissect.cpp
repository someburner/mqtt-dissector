#include <cstdint>
#include <cstdarg>
#include <iostream>

#include "debug.h"

/* for ip conversions */
#include <arpa/inet.h>

/* libtins */
#include "tins/tcp_ip/stream_follower.h"
#include "tins/sniffer.h"
#include "tins/packet.h"
#include "tins/rawpdu.h"
#include "tins/ip.h"
#include "tins/dns.h"
#include "tins/tcp.h"
#include "tins/udp.h"
#include "tins/ip_address.h"
#include "tins/ipv6_address.h"
#include "tins/memory_helpers.h"
#include "tins/network_interface.h"

#include "include/tins_utils.hpp"

/* project includes */
#include "dissect.hpp"
#include "mqtt.hpp"

/* dump options */
#include "options.hpp"

using Tins::Sniffer;
using Tins::SnifferConfiguration;
using Tins::PDU;
using Tins::IP;
using Tins::TCP;
using Tins::UDP;
using Tins::DNS;
using Tins::RawPDU;
using Tins::TCPIP::StreamFollower;
using Tins::TCPIP::Stream;

typedef std::vector<Tins::Sniffer *> SnifferList;

/* tins_utils - global list */
extern std::unique_ptr<TU::IfaceInfo> ifinfo;

TU::ConnPairs cPairs;

static bool colors_enabled = true;
static bool log_redirect   = false;

#if LOGURU_EN
#define LOGURU_IMPLEMENTATION 1
#include "include/loguru.hpp"
static std::string logpath;
static std::string current_logfile;

void enable_logging(std::string& path) {
    printf("Turning on logs using path: %s\n", path.c_str());
    log_redirect = true;
    colors_enabled = false;
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF; // Turn off out > stderr
    loguru::g_preamble_thread = false; // Turn off thread
    loguru::g_preamble_uptime = false; // Turn off uptime
    loguru::g_preamble_file = false;   // Turn off file
    logpath = path;
}
#endif /* LOGURU_EN */

/* Forward declarations */
static bool handle_mqtt_pkt( bool isClient, uint8_t * data, size_t pktlen);

void on_client_data(Stream& stream)
{
    // client_addr_v4
    /* Construct a string out of the contents of the client's payload */
    const Stream::payload_type& client_payload = stream.client_payload();
    size_t datalen = stream.client_payload().end() - stream.client_payload().begin();
    std::string data(stream.client_payload().cbegin(), stream.client_payload().cend());

    c_paintf_cya_bld("CLIENT"); c_paintf_nrm(" >> ");

    // std::cout << TU::client_GetStreamIP(stream) << " >> " << TU::server_GetStreamIP(stream) << std::endl;
    handle_mqtt_pkt( true, (uint8_t*) data.c_str(), data.length() );
}

/* TODO: turn into a single function that just uses arithmatic */
inline void paint_index(int index, char * s) {
    switch(index) {
        case 0: c_paintf_wht_grn("%s", s); break;
        case 1: c_paintf_wht_blu("%s", s); break;
        case 2: c_paintf_wht_ylw("%s", s); break;
        case 3: c_paintf_wht_pur("%s", s); break;
        case 4: c_paintf_wht_cya("%s", s); break;
        case 5: c_paintf_wht_red("%s", s); break;
        case 6: c_paintf_wht_blk("%s", s); break;
        case 7: c_paintf_blk_grn("%s", s); break;
        default: c_paintf_wht_blk("%s", s); break;
    }
}

void on_server_data(Stream& stream)
{
    const Stream::payload_type& server_payload = stream.server_payload();
    size_t datalen = stream.server_payload().end() - stream.server_payload().begin();
    std::string data(stream.server_payload().cbegin(), stream.server_payload().cend());
    c_paintf_blu_bld("SERVER"); c_paintf_nrm(" << ");
    if(!datalen) {
        c_paintf_wht_bld("0 len packet?\n");
        return;
    }
    // std::cout << TU::server_GetStreamIP(stream) << " >> " << TU::client_GetStreamIP(stream) << std::endl;
    // printf("\n data len = %d, datalen = %d\n", data.length(), datalen );
    handle_mqtt_pkt( false, (uint8_t*) data.c_str(), data.length() );
}

static void addRmStream(Stream& s, bool adding) {
    // printf(" ");
    if (adding) { c_paintf_grn_bld(" +++"); } else { c_paintf_red_bld(" ---"); }
    printf(" ");
    /* client */
    uint32_t client4 = Tins::IPv4Address( s.client_addr_v4() );
    uint16_t cport = s.client_port();
    auto chw = s.client_hw_addr();
    /* server */
    uint32_t server4 = Tins::IPv4Address( s.server_addr_v4() );
    uint16_t sport = s.server_port();
    const char * serverName = TU::ip_hex_to_name(server4);

    /* Create id string. Format will look like this:
     * 00:00:00:00:00:00-127.0.0.1-localhost
    */
    char id_buf[64];
    memcpy(&id_buf[0], chw.to_string().c_str(), 17); // 00:00:00:00:00:00
    int remlen = 17;
    remlen += sprintf(id_buf+remlen, "-%hu.%hu.%hu.%hu-%s",
                                (uint8_t)(client4), (uint8_t)(client4>>8),
                                (uint8_t)(client4>>16), (uint8_t)(client4>>24),
                                serverName);
    id_buf[remlen] = '\0';
    std::size_t str_hash = std::hash<std::string>{}(id_buf);

    int pindex = 0;
    if (adding) {
        pindex = cPairs.push( str_hash, id_buf );
    } else {
        pindex = cPairs.pop( str_hash, id_buf );
    }
    paint_index(pindex, id_buf);
    printf("\n");
}

void on_connection_closed(Stream& stream) {
    addRmStream(stream, false);
    logwarn("[-] Connection closed"); //  TU::getStreamIDStr(stream).c_str()
}

void on_new_connection(Stream& stream) {
    /*Disables auto-deleting the client's data after the callback is executed */
    //stream.auto_cleanup_client_data(true);
    /* Same thing for the server's data */
    //stream.auto_cleanup_server_data(true);
    /* Or a shortcut to doing this for both: */
    // stream.auto_cleanup_payloads(false);
    stream.auto_cleanup_payloads(true);
    // bool ConnPairs::push(std::string interface, std::string sip, std::string shw, int sport, std::string dip)

    if (stream.is_partial_stream()) {
        // See Stream::enable_recover_mode for more information
        logwarn("[+] New connection (partial)"); // , TU::getStreamIDStr(stream).c_str()
        // Enable recovery mode using a window of 4kb
        // stream.enable_recovery_mode(4 * 1024);
    }
    else {
        // Print some information about the new connection
        logwarn("[+] New connection"); // TU::getStreamIDStr(stream).c_str()
    }
    addRmStream(stream, true);

    // Now configure the callbacks on it.
    // First, we want on_client_data to be called every time there's new client data
    stream.client_data_callback(&on_client_data);

    // Same thing for server data, but calling on_server_data
    stream.server_data_callback(&on_server_data);

    // When the connection is closed, call on_connection_closed
    stream.stream_closed_callback(&on_connection_closed);
}


bool handle_mqtt_pkt(bool isClient, uint8_t * pktbuf, size_t pktlen)
{
    size_t nread = 0;
    int pos = 0;

    MQTT::PktData pkt( isClient, pktbuf[pos] );
    // printf("parser running (%zu bytes type = %02X)\n", pktlen, type);
    pkt.dumpMQTTType();
    if (pkt.type() == MQTT_CTRL_PUBLISH) {
        uint8_t flags = (pktbuf[pos] & 0x0F);
        pkt.setFlags( flags );
        pkt.dumpFlags();
    }
    pos++;

    uint32_t rlen = 0;
    unsigned mult = 1;
    uint8_t tmp;
    do {
        tmp = pktbuf[ pos++ ];
        rlen += (tmp & 0x7F) * mult;
        mult *= 128;
        if (mult > 128UL*128UL*128UL) {
            c_paintf_red("malformed remaining length!\n");
            return true;
        }
    }
    while (((tmp & 0x80) != 0) && (pos < pktlen));

    uint32_t typeFlag = 1U << ((uint32_t)pkt.type());
    /* Print msg len for PRINT_MSG_SZ_TYPES */
    if (typeFlag & PRINT_MSG_SZ_TYPES) {
        printf("[");
        c_paintf_wht_bld( (pkt.type() == MQTT_CTRL_PUBLISH) ? "%04u" : "%u", rlen);
        printf("]");
    }

    /* Control messages requiring payload */
    MQTT::mqtt_parse_ctrl_packet( pkt, pktbuf, &pos, (int)pktlen );

    /* Control messages requiring packet Identifier */
    pkt.parse_packet_id( pktbuf, &pos, (int)pktlen );
    if(pkt.type() == MQTT_CTRL_PUBACK) {
        pkt.dumpId();
    }

    printf("\n");

    return true;
}

bool dns_cb(const PDU& pdu) {
    /* The packet probably looks likes:
     *               EthernetII / IP / UDP / RawPDU
     * So retrieve the RawPDU layer + construct a DNS PDU using its contents: */
    DNS dns = pdu.rfind_pdu<RawPDU>().to<DNS>();
    /* etrieve the queries and print the domain name: */
    for (const auto& query : dns.queries()) {
        logwarn("DNS: %s", query.dname().c_str() );
    }
    return true;
}

/*
NOTE: filter docs
https://www.tcpdump.org/manpages/pcap-filter.7.html
*/

#define MQTT_PORT_FILTER "(tcp port 1883)"
#define DNS_PORT_FILTER  "(udp and dst port 53)"
#define BOTH_PORT_FILTER "(" MQTT_PORT_FILTER " || " DNS_PORT_FILTER ")"

int dissect(const char* iface, FilterList &flist, OPTIONS_T &opts)
{
    std::string filter = "(";
    if (opts.en_dns) {
        filter += std::string(BOTH_PORT_FILTER);
    } else {
        filter += std::string(MQTT_PORT_FILTER);
    }

    using FilterIter = std::vector<Filter_t*>::const_iterator;
    std::cout << "Addtl filters: \n";
    int filter_len = 0;
    for (FilterIter it = flist.begin(); it!=flist.end(); ++it) {
        if ((*it)->type == FILTER_MAC_T) {
            std::cout << "-> MAC: ";
            filter += " && (ether host "; // ether dst || ether src
        } else if  ((*it)->type == FILTER_IP_T) {
            std::cout << "->  IP: ";
            filter += " && (ip host ";  // ip src || ip dst
        } else {
            continue;
        }
        filter_len++;
        filter += (*it)->param;
        std::cout << (*it)->param << "\n";
        filter += ")";
    }
    if (!filter_len) {
        std::cout << "-> None\n";
    }

    filter += ")";
    logdebug("Filter string =\"%s\"", filter.c_str() );

    MQTT::dump_payloads = opts.dump_payloads;

    try {
        SnifferConfiguration c1;

        c1.set_immediate_mode(true);
        c1.set_promisc_mode(true);
        c1.set_filter(filter.c_str());
        Sniffer s1( iface , c1);
        // s1.set_extract_raw_pdus(true);

        /* Now construct the stream follower */
        StreamFollower follower;
        follower.new_stream_callback(&on_new_connection);

        // process already running TCP streams, if enabled
        if (opts.dis_partials) {
            follower.follow_partial_streams(false);
            std::cout << "Partials disabled\n";
        } else {
            follower.follow_partial_streams(true);
            std::cout << "Partials enabled\n";
        }

        /* Capture. Every time there's a new packet.. */
        s1.sniff_loop([&](PDU& pdu) {
            if ( pdu.find_pdu<TCP>() ) {
                follower.process_packet(pdu); // do process_packet for TCP
            } else {
                dns_cb(pdu); // dns otherwise
            }
            return true;
        });
    } catch (std::exception& ex) {
        logerror("Error: %s", ex.what() );
    }

    return 0;
}

bool validate_iface(std::string& check_if) {
    if ( TU::ifinfo->valid_name(check_if) )
        return true;
    logwarn("invalid interface \"%s\" (run with -s for list)\n", check_if.c_str() );
    return false;
}

void dissect_help(void) {
    TU::ifinfo->dump_shortlist();
}
