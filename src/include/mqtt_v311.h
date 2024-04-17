#ifndef MQTTv311__h
#define MQTTv311__h

#ifdef __cplusplus
extern "C" {
#endif


#define MQTT_CTRL_TYPES_MAP(XX) \
   XX(0,  INVALID     ) \
   XX(1,  CONNECT     ) \
   XX(2,  CONNACK     ) \
   XX(3,  PUBLISH     ) \
   XX(4,  PUBACK      ) \
   XX(5,  PUBREC      ) \
   XX(6,  PUBREL      ) \
   XX(7,  PUBCOMP     ) \
   XX(8,  SUBSCRIBE   ) \
   XX(9,  SUBACK      ) \
   XX(10, UNSUBSCRIBE ) \
   XX(11, UNSUBACK    ) \
   XX(12, PINGREQ     ) \
   XX(13, PINGRESP    ) \
   XX(14, DISCONNECT  )

#define MQTT_CTRL_TYPES_MAX  15

enum mqtt_ctrl_types
{
#define XX(num, unames) MQTT_CTRL_##unames = num,
   MQTT_CTRL_TYPES_MAP(XX)
#undef XX
};

/* Section 2.3.1 pg 20 - Control Packets that contian a Packet Identifier */
#define MQTT_CTRL_EXPECTS_ID (\
(1<<MQTT_CTRL_PUBACK) | (1<<MQTT_CTRL_PUBREC) | \
(1<<MQTT_CTRL_PUBREL)  | (1<<MQTT_CTRL_PUBCOMP) | (1<<MQTT_CTRL_SUBSCRIBE) | \
(1<<MQTT_CTRL_SUBACK)  | (1<<MQTT_CTRL_UNSUBSCRIBE) | (1<<MQTT_CTRL_UNSUBACK) \
)
// NOTE: (1<<MQTT_CTRL_PUBLISH) - special case for qos > 0 only

/* Section 2.4 pg 21 - Control Packets that contian a Payload */
#define MQTT_CTRL_EXPECTS_CTRL_PAYLOAD (\
(1<<MQTT_CTRL_CONNECT) | (1<<MQTT_CTRL_SUBSCRIBE) | (1<<MQTT_CTRL_SUBACK) | \
(1<<MQTT_CTRL_UNSUBSCRIBE) \
)
// NOTE: (1<<MQTT_CTRL_PUBLISH) - optional

typedef struct {
    char * topic;
    uint16_t topic_qos;
    uint16_t topic_len;
} MQTT_Topic_t;


/* -------------------------------------------------------------------------- */


/* Notes: KEEPALIVE
 * It is the responsibility of the Client to ensure that the interval between
 * Control Packets being sent does not exceed the Keep Alive value. In the
 * absence of other Control Packets, the Client MUST send a PINGREQ Packet.
 *
 * The maximum keepalive value is 18 hours 12 minutes and 15 seconds.
*/


/* Notes: CLEAN SESSION
 * When a Client connects with CleanSession set to 0, it is requesting that the
 * Server maintain its MQTT session state after it disconnects. Clients should
 * only connect with CleanSession set to 0, if they intend to reconnect to the
 * Server at some later point in time. When a Client has determined that it has
 * no further use for the session it should do a final connect with CleanSession
 * set to 1 and then disconnect.
*/


/* Notes: CLIENT ID / USER / PASS
 * The Server MUST allow ClientIds which are between 1 and 23 UTF-8 encoded
 * bytes in length, and that contain only the characters:
 * "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
 *
 * If the User Name Flag is set to 0, the Password Flag MUST be set to 0
*/


/* Notes: SUBSCRIBE
 * Bits 3,2,1 and 0 of the fixed header of the SUBSCRIBE Control Packet are
 * reserved and MUST be set to 0,0,1 and 0 respectively. The Server MUST treat
 * any other value as malformed and close the Network Connection.
 *
 * A SUBSCRIBE packet with no payload is a protocol violation.
 *
 * The Server MUST treat a SUBSCRIBE packet as malformed and close the Network
 * Connection if any of Reserved bits in the payload (upper 6 bits of last byte)
 * are non-zero, or QoS is not 0,1 or 2.
*/


/* Notes: PUBLISH
 * The Topic Name in the PUBLISH Packet MUST NOT contain wildcard characters.
 *
 * The DUP flag MUST be set to 1 by the Client or Server when it attempts to
 * re-deliver a PUBLISH Packet. DUP MUST be set to 0 for all QoS 0 messages
 *
 * The value of the DUP flag from an incoming PUBLISH packet is not propagated
 * when the PUBLISH Packet is sent to subscribers by the Server. The DUP flag in
 * the outgoing PUBLISH packet is set independently to the incoming PUBLISH
 * packet, its value MUST be determined solely by whether the outgoing PUBLISH
 * packet is a retransmission
*/



#ifdef __cplusplus
}
#endif

#endif /* MQTTv311__h */
