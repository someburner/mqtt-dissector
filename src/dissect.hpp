#ifndef DISSECT__hpp
#define DISSECT__hpp
#include <chrono>
#include <vector>

#include "options.hpp"

inline uint64_t epoch_secs_now() {
    return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1000);
}

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
    uint64_t    _prev_client_secs, _prev_server_secs;
    uint64_t    _cur_client_secs, _cur_server_secs;

    uint64_t    _prev_ping_secs, _cur_ping_secs;

public:
    UserData() {
        _start_secs = 0;
        _prev_ping_secs = 0;
    }
    ~UserData() { }

    void SetPingSecs(uint64_t epoch) {
        _prev_ping_secs = _cur_ping_secs;
        _cur_ping_secs = epoch;
    }

    uint64_t GetPingDelta(void) {
        return _cur_ping_secs - _prev_ping_secs;
    }

    void SetStartSecs(uint64_t epoch) {
        if (_start_secs == 0) {
            _start_secs = _cur_client_secs = epoch;
        }
    }

    void SetCurSecs(bool isClient) {
        if (isClient) {
            _prev_client_secs = _cur_client_secs;
            _cur_client_secs = epoch_secs_now();
        } else {
            _prev_server_secs = _cur_server_secs;
            _cur_server_secs = epoch_secs_now();
        }
    }

    uint64_t GetPktDelta(bool isClient) {
        if (isClient) {
            return _cur_client_secs - _prev_client_secs;
        } else {
            return _cur_server_secs - _prev_server_secs;
        }
    }

    uint64_t GetCurSecs(bool isClient) {
        if (isClient) {
            return _cur_client_secs;
        } else {
            return _cur_server_secs;
        }
    }

    uint64_t GetElapsedSecs(bool isClient) {
        if (isClient) {
            return _cur_client_secs - _start_secs;
        } else {
            return _cur_server_secs - _start_secs;
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
