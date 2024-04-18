#ifndef DISSECT__hpp
#define DISSECT__hpp
#include <vector>

#include "options.hpp"

enum {
    FILTER_INVALID_T,
    FILTER_MAC_T,
    FILTER_IP_T
};

typedef struct {
    int type;
    std::string param;
} Filter_t;

class UserData {
private:
    bool  _isClient;
    std::string client_id;

    uint64_t    _start_secs;
    uint64_t    _last_msg_secs;

public:
    UserData() {
        _start_secs = 0;
    }
    ~UserData() { }

    void SetStartSecs(uint64_t epoch) {
        if (_start_secs == 0) {
            _start_secs = epoch;
            _last_msg_secs = epoch;
        }
    }

    uint64_t GetStartSecs() {
        return _start_secs;
    }
};

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

int dissect(const char* iface, FilterList &flist, OPTIONS_T &opts);
bool validate_iface(std::string& check_if);
void dissect_help(void);


#endif /* DISSECT__hpp */
