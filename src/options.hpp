#ifndef OPTIONS__hpp
#define OPTIONS__hpp

typedef struct OPTIONS_T {
    bool en_dns;
    bool dis_partials;
    bool dump_payloads;
    bool dump_stamps;
    bool dump_deltas;
} OPTIONS_T;

#define PRINT_MSG_SZ_TYPES ( \
    (1U << MQTT_CTRL_PUBLISH) \
)

// | (1U << MQTT_CTRL_SUBSCRIBE)

#endif /* OPTIONS__hpp */
