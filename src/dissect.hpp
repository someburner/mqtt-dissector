#ifndef DISSECT__hpp
#define DISSECT__hpp
#include <vector>

enum {
    FILTER_INVALID_T,
    FILTER_MAC_T,
    FILTER_IP_T
};

typedef struct {
    int type;
    std::string param;
} Filter_t;

typedef std::vector<Filter_t*> FilterList;

inline void print_buf_chars(uint8_t * b, unsigned len) {
    printf("[%u] >|", len);
    for (int i=0; i<len; i++) {
        uint8_t c = b[i];
        if ((c >= ' ') && ((c <= '~')))
            printf(" %c|", (char)c );
        else
            printf("%02X ", c);
    }
}

inline void print_buf_hex(uint8_t * b, unsigned len) {
    printf("[%u] > ", len);
    for (int i=0; i<len; i++) { printf("%02X ", b[i]); }
}

int dissect(const char* iface, FilterList &flist, bool en_dns, bool payloads);
bool validate_iface(std::string& check_if);
void dissect_help(void);


#endif /* DISSECT__hpp */
