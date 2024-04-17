#ifndef MQTT_DISSECTOR_VERSION__hpp
#define MQTT_DISSECTOR_VERSION__hpp

#define STRINGIZE( STR ) #STR
#define MAKE_STRING( STR ) STRINGIZE( STR )

#define V_MAJOR 0
#define V_MINOR 0
#define V_SUB   2

inline const char * get_version(void) {
    return "v" MAKE_STRING(V_MAJOR) "." MAKE_STRING(V_MINOR) "." MAKE_STRING(V_SUB) "";
}

inline const char * get_pcap_version(void) {
    extern char pcap_version[];
    return (const char *) pcap_version;
}

inline const char * get_tins_version(void) {
    #include <tins/config.h>
    return "v" MAKE_STRING(TINS_VERSION_MAJOR) "." MAKE_STRING(TINS_VERSION_MINOR) "." MAKE_STRING(TINS_VERSION_PATCH) "";
}

#endif /* MQTT_DISSECTOR_VERSION__hpp */
