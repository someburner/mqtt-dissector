#ifndef DEBUG__h_
#define DEBUG__h_

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------- options --------------------------------- */
#ifndef EN_DEVMODE
#define EN_DEVMODE 0
#endif

#if EN_DEVMODE
    #define DEBUG_ENABLED
    #define _printf          printf
#else
    #define _printf(...)     do {} while(0)
#endif

/* Color codes for enhanced debugging. */
static const char _nrm[] = {0x1b, '[', '0', ';', '3', '9', 'm', 0}; /* normal/reset */
static const char _blk[] = {0x1b, '[', '1', ';', '3', '0', 'm', 0}; /* black */
static const char _red[] = {0x1b, '[', '1', ';', '3', '1', 'm', 0}; /* red */
static const char _grn[] = {0x1b, '[', '0', ';', '3', '2', 'm', 0}; /* green */
static const char _ylw[] = {0x1b, '[', '1', ';', '3', '3', 'm', 0}; /* yellow */
static const char _blu[] = {0x1b, '[', '1', ';', '3', '4', 'm', 0}; /* blue */
static const char _pur[] = {0x1b, '[', '0', ';', '3', '5', 'm', 0}; /* purple */
static const char _cya[] = {0x1b, '[', '0', ';', '3', '6', 'm', 0}; /* cyan */
static const char _wht[] = {0x1b, '[', '0', ';', '0', '1', 'm', 0}; /* bold white */
static const char _udl[] = {0x1b, '[', '0', ';', '0', '4', 'm', 0}; /* underlined */
static const char _inv[] = {0x1b, '[', '0', ';', '0',  '7', 'm', 0}; /* inverted/shaded */

#define CLR_OFFETS_MAP(XX) \
    XX(0,  BLK, blk, '0' ) \
    XX(1,  RED, red, '1'  ) \
    XX(2,  GRN, grn, '2'  ) \
    XX(3,  YLW, ylw, '3'  ) \
    XX(4,  BLU, blu, '4'  ) \
    XX(5,  PUR, pur, '5'  ) \
    XX(6,  CYA, cya, '6'  ) \
    XX(7,  WHT, wht, '7'  ) \
    XX(8,  GRY, gry, '8'  ) \
    XX(9,  NRM, nrm, '9'  )

/* Standard color set */
#define XX(num, unames, lnames, fch) static const char _##lnames##_std[] = {0x1b,'[', '0', ';', '3', fch, 'm', 0};
CLR_OFFETS_MAP(XX)
#undef XX

/* Bright color set */
#define XX(num, unames, lnames, fch) static const char _##lnames##_bri[] = {0x1b,'[', '0', ';', '9', fch, 'm', 0};
CLR_OFFETS_MAP(XX)
#undef XX

/* bold ... */
#define XX(num, unames, lnames, fch) static const char _##lnames##_bld[] = {0x1b,'[', '1', ';', '9', fch, 'm', 0};
CLR_OFFETS_MAP(XX)
#undef XX

/* black on ... */
#define XX(num, unames, lnames, fch) static const char blk_on_##lnames##_[] = {0x1b,'[', '7', ';', '3', fch, ';', '4', '4', 'm', 0};
CLR_OFFETS_MAP(XX)
#undef XX

/* white on ... */
#define XX(num, unames, lnames, fch) static const char wht_on_##lnames##_[] = {0x1b,'[', '0', ';', '3', '9', ';', '4', fch, 'm', 0};
CLR_OFFETS_MAP(XX)
#undef XX

/* bold white on ... */
#define XX(num, unames, lnames, fch) static const char bwht_on_##lnames##_[] = {0x1b,'[', '1', ';', '3', '9', ';', '4', fch, 'm', 0};
CLR_OFFETS_MAP(XX)
#undef XX

/* ... on blu */
#define XX(num, unames, lnames, fch) static const char _##lnames##_on_blu[] = {0x1b,'[', '1', ';', '3', fch, ';', '4', '4', 'm', 0};
CLR_OFFETS_MAP(XX)
#undef XX

#define c_paintf(color, ... ) \
do { \
    _printf("%s", color); _printf(__VA_ARGS__); _printf("%s", (const char*)_nrm); \
} while(0)

/* standard */
#define c_paintf_nrm(...)    c_paintf(_nrm_std, __VA_ARGS__)
#define c_paintf_red(...)    c_paintf(_red_std, __VA_ARGS__)
#define c_paintf_grn(...)    c_paintf(_grn_std, __VA_ARGS__)
#define c_paintf_blk(...)    c_paintf(_blk_std, __VA_ARGS__)
#define c_paintf_ylw(...)    c_paintf(_ylw_std, __VA_ARGS__)
#define c_paintf_blu(...)    c_paintf(_blu_std, __VA_ARGS__)
#define c_paintf_pur(...)    c_paintf(_pur_std, __VA_ARGS__)
#define c_paintf_cya(...)    c_paintf(_cya_std, __VA_ARGS__)
#define c_paintf_wht(...)    c_paintf(_wht_std, __VA_ARGS__)
/* bright */
#define c_paintf_bri_nrm(...)    c_paintf(_nrm_bri, __VA_ARGS__)
#define c_paintf_bri_red(...)    c_paintf(_red_bri, __VA_ARGS__)
#define c_paintf_bri_grn(...)    c_paintf(_grn_bri, __VA_ARGS__)
#define c_paintf_bri_blk(...)    c_paintf(_blk_bri, __VA_ARGS__)
#define c_paintf_bri_ylw(...)    c_paintf(_ylw_bri, __VA_ARGS__)
#define c_paintf_bri_blu(...)    c_paintf(_blu_bri, __VA_ARGS__)
#define c_paintf_bri_pur(...)    c_paintf(_pur_bri, __VA_ARGS__)
#define c_paintf_bri_cya(...)    c_paintf(_cya_bri, __VA_ARGS__)
#define c_paintf_bri_wht(...)    c_paintf(_wht_bri, __VA_ARGS__)
/* bold */
#define c_paintf_nrm_bld(...)    c_paintf(_nrm_bld, __VA_ARGS__)
#define c_paintf_red_bld(...)    c_paintf(_red_bld, __VA_ARGS__)
#define c_paintf_grn_bld(...)    c_paintf(_grn_bld, __VA_ARGS__)
#define c_paintf_blk_bld(...)    c_paintf(_blk_bld, __VA_ARGS__)
#define c_paintf_ylw_bld(...)    c_paintf(_ylw_bld, __VA_ARGS__)
#define c_paintf_blu_bld(...)    c_paintf(_blu_bld, __VA_ARGS__)
#define c_paintf_pur_bld(...)    c_paintf(_pur_bld, __VA_ARGS__)
#define c_paintf_cya_bld(...)    c_paintf(_cya_bld, __VA_ARGS__)
#define c_paintf_wht_bld(...)    c_paintf(_wht_bld, __VA_ARGS__)
/* blk on ... */
#define c_paintf_blk_nrm(...)    c_paintf(blk_on_nrm_, __VA_ARGS__)
#define c_paintf_blk_red(...)    c_paintf(blk_on_red_, __VA_ARGS__)
#define c_paintf_blk_grn(...)    c_paintf(blk_on_grn_, __VA_ARGS__)
#define c_paintf_blk_blk(...)    c_paintf(blk_on_blk_, __VA_ARGS__)
#define c_paintf_blk_ylw(...)    c_paintf(blk_on_ylw_, __VA_ARGS__)
#define c_paintf_blk_blu(...)    c_paintf(blk_on_blu_, __VA_ARGS__)
#define c_paintf_blk_pur(...)    c_paintf(blk_on_pur_, __VA_ARGS__)
#define c_paintf_blk_cya(...)    c_paintf(blk_on_cya_, __VA_ARGS__)
#define c_paintf_blk_wht(...)    c_paintf(blk_on_wht_, __VA_ARGS__)
/* wht on ... */
#define c_paintf_wht_nrm(...)    c_paintf(wht_on_nrm_, __VA_ARGS__)
#define c_paintf_wht_red(...)    c_paintf(wht_on_red_, __VA_ARGS__)
#define c_paintf_wht_grn(...)    c_paintf(wht_on_grn_, __VA_ARGS__)
#define c_paintf_wht_blk(...)    c_paintf(wht_on_blk_, __VA_ARGS__)
#define c_paintf_wht_ylw(...)    c_paintf(wht_on_ylw_, __VA_ARGS__)
#define c_paintf_wht_blu(...)    c_paintf(wht_on_blu_, __VA_ARGS__)
#define c_paintf_wht_pur(...)    c_paintf(wht_on_pur_, __VA_ARGS__)
#define c_paintf_wht_cya(...)    c_paintf(wht_on_cya_, __VA_ARGS__)

#define print_tag(color, msg) \
do { \
    _printf("["); _printf("%s", color); _printf("%s", msg); _printf("%s", (const char*)_nrm); _printf("] "); \
} while(0)

#define logdebug(...) \
do { \
    print_tag( (const char*)_blu, "DEBUG"); \
    _printf(__VA_ARGS__); \
    _printf("\n"); \
} while(0)

#define loginfo(...) \
do { \
    print_tag( (const char*)_wht, "INFO"); \
    _printf(__VA_ARGS__); \
    _printf("\n"); \
} while(0)

#define logerror(...) \
do { \
    print_tag( (const char*)_red, "ERROR"); \
    _printf(__VA_ARGS__); \
    _printf("\n"); \
} while(0)

#define logwarn(...) \
do { \
    print_tag( (const char*)_ylw, "WARN"); \
    _printf(__VA_ARGS__); \
    _printf("\n"); \
} while(0)

#define logokay(...) \
do { \
    print_tag( (const char*)_grn, "OKAY"); \
    _printf(__VA_ARGS__); \
    _printf("\n"); \
} while(0)



#ifdef __cplusplus
}
#endif

#endif /* DEBUG__h_ */
