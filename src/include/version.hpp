#ifndef MQTT_DISSECTOR_VERSION__hpp
#define MQTT_DISSECTOR_VERSION__hpp

#define STRINGIZE( STR ) #STR
#define MAKE_STRING( STR ) STRINGIZE( STR )

#if defined(GIT_BRANCH) && defined(GIT_SHORT_SHA) && defined(BUILD_DATE)
inline const char * get_version(void)
{
    return "mqtt-dissector: " MAKE_STRING(GIT_BRANCH) "/" MAKE_STRING(GIT_SHORT_SHA) " (built " MAKE_STRING(BUILD_DATE) ")";
}
#else
inline const char * get_version(void)
{
    return "mqtt-dissector: unknown version";
}
#endif

inline const char * get_pcap_version(void) {
    extern char pcap_version[];
    return (const char *) pcap_version;
}

inline const char * get_tins_version(void) {
    #include <tins/config.h>
    return "v" MAKE_STRING(TINS_VERSION_MAJOR) "." MAKE_STRING(TINS_VERSION_MINOR) "." MAKE_STRING(TINS_VERSION_PATCH) "";
}

#endif /* MQTT_DISSECTOR_VERSION__hpp */
