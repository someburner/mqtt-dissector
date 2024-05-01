#ifndef MQTT__hpp
#define MQTT__hpp

#include "include/mqtt_v311.h"
#include "dump.hpp"

#define CF_CLIENT_ID 0
#define CF_RESERVED 1
#define CF_CLN_SESS 2
#define CF_WILL_TOP 3  // topic + msg
#define CF_WILL_MSG 4  // topic + msg
#define CF_WILL_QOS 5
#define CF_WILL_RET 6
#define CF_PASSWORD 7
#define CF_USERNAME 8


/* MQTT - Mosquitto */
namespace MQTT
{

bool dump_payloads = false;

/* ----------------------------- Base Types ------------------------------ */
typedef union {
    uint8_t       val;
    struct {
        /* 3.1.2.3 - connect flags */
        uint8_t   reserved:1;
        uint8_t   clean_session:1;
        uint8_t   will_flag:1;
        uint8_t   will_qos:2;
        uint8_t   will_retain:1;
        uint8_t   password:1;
        uint8_t   username:1;
    } get;
} ConnectFlags_t;

typedef struct {
    uint16_t            nameID; // 1-2
    char          nameBytes[4]; // 3-6
    uint8_t          proto_lvl; // 7
    ConnectFlags_t       flags; // 8
    uint16_t        keep_alive; // 9-10

public:
    bool reserved(void) { return this->flags.get.reserved; }
    bool clean_session(void) { return this->flags.get.clean_session; }
    bool will(void) { return this->flags.get.will_flag; }
    int  will_qos(void) { return this->flags.get.will_qos; }
    bool will_retain(void) { return this->flags.get.will_retain; }
    bool hasPass(void) { return this->flags.get.password; }
    bool hasUser(void) { return this->flags.get.username; }
} OnConnectFixedData_t;


/* ---------------------------- Generic Methods ---------------------------- */
inline uint16_t mqtt_extract_uint16(uint8_t * pktbuf, int * pos) {
    uint16_t u16out = 0xFF00 & ((uint16_t)pktbuf[ (*pos)++ ] << 8);
    u16out |= 0x00FF & ((uint16_t)pktbuf[ (*pos)++ ]);
    // printf("\nu16out=%u\n", u16out);
    return u16out;
}

inline char * mqtt_extract_string(uint8_t * pktbuf, int * pos) {
    int ptmp = *pos;
    uint16_t len = ((uint16_t)pktbuf[ ptmp++ ] << 8);
    len |= (uint16_t)pktbuf[ ptmp++ ];
    // printf("extract [%d] len=%u\n", ptmp, len);
    char * tmp = new char[len+1];
    memcpy( tmp, pktbuf+ptmp, len);
    ptmp += len;
    tmp[len] = 0;
    *pos = ptmp;
    return tmp;
}

inline void mqtt_dump_ctrl_connect_header(OnConnectFixedData_t& ci)
{
    printf("|%hu|%c%c%c%c|L%02X|F%02X|K%hu|\n", ci.nameID,
        ci.nameBytes[0], ci.nameBytes[1], ci.nameBytes[2], ci.nameBytes[3],
        ci.proto_lvl, ci.flags.val, ci.keep_alive
    );
    printf("|------------ FLAGS ---------------|\n");
    printf("| un | pw | wr | wq | wf | cs |  r |\n");
    printf("| %2d | %2d | %2d | %2d | %2d | %2d | %2d |\n",
        ci.flags.get.username, ci.flags.get.password, ci.flags.get.will_retain,
        ci.flags.get.will_qos, ci.flags.get.will_flag, ci.flags.get.clean_session,
        ci.flags.get.reserved
    );
}

const char * mqtt_get_type_str(uint8_t t)
{
    switch (t) {
        case MQTT_CTRL_INVALID:     return "INVALID";
        case MQTT_CTRL_CONNECT:     return "CONNECT";
        case MQTT_CTRL_CONNACK:     return "CONNACK";
        case MQTT_CTRL_PUBLISH:     return "PUBLISH";
        case MQTT_CTRL_PUBACK:      return "PUBACK";
        case MQTT_CTRL_PUBREC:      return "PUBREC";
        case MQTT_CTRL_PUBREL:      return "PUBREL";
        case MQTT_CTRL_PUBCOMP:     return "PUBCOMP";
        case MQTT_CTRL_SUBSCRIBE:   return "SUBSCRIBE";
        case MQTT_CTRL_SUBACK:      return "SUBACK";
        case MQTT_CTRL_UNSUBSCRIBE: return "UNSUBSCRIBE";
        case MQTT_CTRL_UNSUBACK:    return "UNSUBACK";
        case MQTT_CTRL_PINGREQ:     return "PINGREQ";
        case MQTT_CTRL_PINGRESP:    return "PINGRESP";
        case MQTT_CTRL_DISCONNECT:  return "DISCONNECT";
    }
    return "N/A";
}

/* ----------------------------- Class ------------------------------ */


typedef std::map< int, std::string > OnConnectVariable_t;

class PktData {
private:
    bool  _isClient;
    bool   _retain;
    uint8_t   _qos;
    bool      _dup;
    uint16_t  _id;
    uint8_t   _type;

    OnConnectFixedData_t _flags;    // onConnect - fixed
    OnConnectVariable_t  _mqttInfo; // onConnect - variable len key-pairs

public:

    PktData(bool isClient, uint8_t firstByte) {
        _isClient = isClient;
        _type = (( firstByte & 0xF0) >> 0x04);
    }
    ~PktData() { }

    inline void setFlags(uint8_t flags) {
        _dup =  (bool) (flags & (0x01<<3));
        /* server stores retain and delivers to new subscribers on the topic at the QoS set by publisher */
        _retain = (bool) (flags & 0x01);
        _qos = ((flags & 0x6)>>1);
    }
    inline void dumpFlags(void) {
        if ( _dup ) printf(" +DUP");
        if ( _retain ) printf(" +RETAIN");
    }
    inline void dumpId(void) {
        printf("[ID-%u]", _id);
    }

    inline void dumpMQTTType(void) {
        const char * typeStr = mqtt_get_type_str(_type);

        if ( _isClient )
            c_paintf_cya("%s", typeStr );
        else
            c_paintf_blu("%s", typeStr );

        if ( (_type == MQTT_CTRL_INVALID) || (_type >= MQTT_CTRL_TYPES_MAX) ) {
            printf("["); c_paintf_red("ERROR"); printf("]: ");
            printf("Invalid/unknown MQTT msg type detected (%hhu)\n", (_type));
        }
    }

    inline bool isClient() { return _isClient; }

    inline bool dup() { return _dup; }
    inline uint8_t qos() { return _qos; }
    inline bool retain() { return _retain; }
    inline uint8_t type() { return _type; }
    inline uint16_t id() { return _id; }

    inline void parse_ctrl_connect(uint8_t * pktbuf, int * pos, int pkt_len);
    inline void parse_packet_id( uint8_t * pktbuf, int * pos, int pkt_len);
};

/* ----------------------------- Class Methods ------------------------------ */
inline void PktData::parse_packet_id(uint8_t * pktbuf, int * pos, int pkt_len) {
    int ptmp = *pos;
    if((1<<((uint32_t)_type)) & MQTT_CTRL_EXPECTS_ID) {
        _id = ((uint16_t)pktbuf[ ptmp++ ]) << 8;
        _id |= ((uint16_t)pktbuf[ ptmp++ ]);
    } else {
        // printf("not expecting ID\n");
    }
}

inline void PktData::parse_ctrl_connect(uint8_t * pktbuf, int * pos, int pkt_len)
{
    int _pos0 = *pos;
    memset( &_flags, 0, sizeof(_flags) );
    memcpy( &_flags, pktbuf+_pos0, 10);
    *pos = *pos + 10;

    _flags.nameID = __builtin_bswap16( _flags.nameID );
    _flags.keep_alive = __builtin_bswap16( _flags.keep_alive );

    printf("pkt.nameID = %04X\n", _flags.nameID);
    printf("pkt.nameBytes = %c%c%c%c\n",
        _flags.nameBytes[0], _flags.nameBytes[1], _flags.nameBytes[2], _flags.nameBytes[3] );
    printf("pkt.proto_lvl = %02X\n", _flags.proto_lvl);
    printf("pkt.val = %02X\n", _flags.flags.val);
    printf("pkt.keep_alive = %hu\n", _flags.keep_alive );

    /* Dump header (optional / debug) */
    mqtt_dump_ctrl_connect_header( _flags );

    /* (1 - REQUIRED) clientID */
    char * id_str = mqtt_extract_string(pktbuf, pos);
    _mqttInfo.insert( OnConnectVariable_t::value_type( CF_CLIENT_ID, id_str ) );

    /* (2 - OPTIONAL) Will Topic and Will Message fields */
    if ( _flags.will() ) {
        /* NOTE: Both MUST be present in the payload if will_flag is 1 */
        char * wtopic = mqtt_extract_string(pktbuf, pos);
        /* The Will Topic MUST be a UTF-8 encoded string */
        _mqttInfo.insert( OnConnectVariable_t::value_type( CF_WILL_TOP, wtopic ) );
        // printf("WILL TOPIC: %s\n", wtopic); delete wtopic;

        char * wmsg = mqtt_extract_string(pktbuf, pos);
        _mqttInfo.insert( OnConnectVariable_t::value_type( CF_WILL_MSG, wmsg ) );
        // printf("WILL MSG: %s\n", wmsg);  delete wmsg;
    }

    if ( ( _flags.hasUser() ) ^ (_flags.hasPass()) ) {
        c_paintf_red("ERROR"); printf(": Username XOR Password is true!\n");
    }

    /* (3 - OPTIONAL) Username, if set */
    if ( _flags.hasUser() ) {
        char * user = mqtt_extract_string(pktbuf, pos);
        _mqttInfo.insert( OnConnectVariable_t::value_type( CF_USERNAME, user ) );
        // printf("USER: %s\n", user);  delete user;
    }
    /* (4 - OPTIONAL) Password, if set. */
    if (_flags.hasPass()) {
        char * pass = mqtt_extract_string(pktbuf, pos);
        _mqttInfo.insert( OnConnectVariable_t::value_type( CF_PASSWORD, pass ) );
        // printf("PASS: %s\n", pass);  delete pass;
    }

    for (OnConnectVariable_t::const_iterator itr = _mqttInfo.begin(); itr != _mqttInfo.end(); ++itr) {
        std::cout << "[" << itr->first << "] " << itr->second << std::endl;
    }
}

inline int mqtt_parse_ctrl_subscribe(uint8_t * pktbuf, int * pos, int pkt_len)
{
    int parsedTopics = 0;
    int topicPos = *pos;
    // print_buf_hex(pktbuf, pkt_len);

again:
    char * tmpTopic = mqtt_extract_string(pktbuf, &topicPos);
    uint8_t tmpQos = ( 0x03 & (pktbuf[ topicPos ] ) ) ;
    printf(" [QoS-%X] +%s", tmpQos, tmpTopic);
    delete tmpTopic;
    parsedTopics++;
    // *pos = topicPos + 1;
    if ((topicPos+1) < pkt_len) {
        printf("again\n");
        goto again;
    }
    *pos = topicPos;
    // printf("\t+pos end = %d/%d\n", topicPos, pkt_len);
    return parsedTopics;
}

inline int mqtt_parse_ctrl_unsubscribe(uint8_t * pktbuf, int * pos, int pkt_len)
{
    int parsedTopics = 0;
    int topicPos = *pos;
    // print_buf_hex(pktbuf, pkt_len);

again:
    char * tmpTopic = mqtt_extract_string(pktbuf, &topicPos);
    uint8_t tmpQos = ( 0x03 & (pktbuf[ topicPos ] ) ) ;
    printf(" [QoS-%X] -%s", tmpQos, tmpTopic);
    delete tmpTopic;
    parsedTopics++;
    // *pos = topicPos + 1;
    if ((topicPos+1) < pkt_len) {
        printf("again\n");
        goto again;
    }
    *pos = topicPos;
    // printf("\t+pos end = %d/%d\n", topicPos, pkt_len);
    return parsedTopics;
}

inline int mqtt_parse_ctrl_suback(uint8_t * pktbuf, int * pos, int pkt_len)
{
    int qosAck = *pos;
    uint8_t retcode = pktbuf[qosAck];

    printf(" [");
    if (retcode & 0x80) { c_paintf_red("ERR"); }
    else { c_paintf_grn("OK"); }
    printf("] QoS-%X", (retcode & 0x7F) );
    // print_buf_hex(pktbuf, pkt_len);
    (*pos)++;
    return 1;
}

inline int mqtt_parse_ctrl_publish(PktData& pkthdr, uint8_t * pktbuf, int * pos, int pkt_len)
{
    /* NOTE:
     * 1.) It is valid for a PUBLISH Packet to contain a zero length payload
     * 2.) A PUBLISH Packet MUST NOT contain a Packet Identifier if its QoS value is set to 0
    **/
    int startPos = (*pos);
    printf(" [QoS-%X]", pkthdr.qos());
    char * tmpTopic = mqtt_extract_string(pktbuf, &startPos);
    if (pkthdr.qos() > 0) {
        uint16_t tmpPktID = ((uint16_t)pktbuf[ startPos++ ]) << 8;
        tmpPktID |= ((uint16_t)pktbuf[ startPos++ ]);
        printf("[ID-%u]", tmpPktID);
    }
    /* Copy payload */
    int plen = (pkt_len - startPos);
    char * payload = new char[plen+1];
    memcpy( payload, pktbuf+startPos, plen );
    payload[ plen ] = 0;
    /* Update pointer */
    *pos = startPos;
    /* Do stuff */
    printf(" -> %s\t", tmpTopic);
    try {
        Dump::dump_mqtt_json_obj( payload, plen );
    } catch (std::runtime_error& e) {
        c_paintf_red("ERROR: "); std::cout << "[parser] " << e.what() << std::endl;
    }

    if (MQTT::dump_payloads) {
        if ( pkthdr.isClient() )
            c_paintf_cya("\n%s", payload);
        else
            c_paintf_blu("\n%s", payload);
    }

    delete payload;
    delete tmpTopic;

#if 0
    print_buf_hex(pktbuf+startPos, (pkt_len - startPos) ); printf("\n");
    print_buf_chars(pktbuf+startPos, (pkt_len - startPos) );
#endif
    return 1;
}

inline void mqtt_parse_ctrl_packet(PktData& pkthdr, uint8_t * pktbuf, int * pos, int pkt_len) {
    if ( ((1<<((uint32_t)pkthdr.type())) & MQTT_CTRL_EXPECTS_CTRL_PAYLOAD) || (pkthdr.type() == MQTT_CTRL_PUBLISH) )
    {
        switch (pkthdr.type()) {
            case MQTT_CTRL_CONNECT: {
                /* Client Identifier, Will Topic, Will Message, User Name, Password */
                // mqtt_parse_ctrl_connect( pkthdr, pktbuf, pos, pkt_len);
                pkthdr.parse_ctrl_connect( pktbuf, pos, pkt_len  );
            } break;

            case MQTT_CTRL_PUBLISH: {
                int pubct = mqtt_parse_ctrl_publish(pkthdr, pktbuf, pos, pkt_len);
            } break;

            case MQTT_CTRL_SUBSCRIBE: {
                // MQTT_Topic_t ** subs = NULL;
                uint16_t dummy = mqtt_extract_uint16(pktbuf, pos);
                // printf(" (d=%hu) ", dummy);
                int subct = mqtt_parse_ctrl_subscribe(pktbuf, pos, pkt_len);
            } break;

            case MQTT_CTRL_SUBACK: {
                uint16_t dummy = mqtt_extract_uint16(pktbuf, pos);
                // printf(" (d=%hu) ", dummy);
                mqtt_parse_ctrl_suback(pktbuf, pos, dummy);
            } break;

            case MQTT_CTRL_UNSUBSCRIBE: {
                uint16_t dummy = mqtt_extract_uint16(pktbuf, pos);
                mqtt_parse_ctrl_unsubscribe(pktbuf, pos, pkt_len);
            } break;

        }
        // pkthdr.id() = ( (((uint16_t)(pktbuf[*pos])) << 8) |  ((uint16_t)(pktbuf[*pos+1])) );
        // printf("/"); c_paintf_ylw("%hu", pkthdr.id());
        // (*pos)++;
    }
}




} /* MQTT */



#endif /* MQTT__hpp */
