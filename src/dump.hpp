#ifndef DUMP_hpp
#define DUMP_hpp

// TODO
// This work was never really completed
// Main idea is to be able to dump json payloads in a pretty-print fashion

#define DUMP_TYPE_VERBOSE 1
#define DUMP_TYPE_MIN     2
#define DUMP_TYPE_NONE    3

#define DUMP_TYPE   (DUMP_TYPE_NONE)

/* Dump namespace */
namespace Dump
{

#if 0
inline void dump_mqtt_json_obj(char * buf, int len)
{
    picojson::value v;
    std::string err;
    parse(v, buf, buf+len, &err);
    if (err.empty()) {
        // picojson::array arr = v.get<array>();
        picojson::object ob = v.get<object>();
        picojson::object::iterator it;
        // for (it = ob.begin(); it != ob.end(); it++) {
            // object obj = it->get<object>();
            // std::cout << "#" << obj["number"].to_str() << ": " << obj["title"].to_str() << endl;
            std::cout << "#" << ob["testing"].to_str() << std::endl;
        // }
        // for (it = arr.begin(); it != arr.end(); it++) {
        //     object obj = it->get<object>();
        //
        // }
    } else {
        std::cerr << err << std::endl;
    }
    // dump json object
    // std::cout << "---- dump input ----" << std::endl;
    std::cout << v << std::endl;
}
#endif


#if (DUMP_TYPE==DUMP_TYPE_MIN)
/* ---------------------------- minimal/condensed --------------------------- */
inline void dump_mqtt_json_obj(char * buf, int len)
{
    /* TODO */
}


#elif (DUMP_TYPE==DUMP_TYPE_VERBOSE)
/* -------------------------------- vebose/debug ---------------------------- */
inline void print_indent_v( int ct ) {
    for (int k=0; k < ct; k++) { printf ("  "); }
}

#if 0
inline void dump_obj_v( const picojson::object &ob, int indent);

inline void dumb_val_type_v( const picojson::value &v, int indent ) {
    print_indent_v( indent );
    if (v.is<std::string>()) {  printf("(str)"); }
    else if (v.is<double>()) { printf("(dbl)"); }
    else if (v.is<picojson::object>()) { printf("(obj)"); }
    else if (v.is<bool>()) { printf("(bool)"); }
    else if (v.is<picojson::null>()) { printf("(NULL)"); }
    else if (v.is<picojson::array>()) { printf("(arr)"); }
    else {
        printf("(UNKNOWN)");
    }
}

inline void dump_item_v( picojson::value v, int * indent ) {
    // std::string(buf, len) >> v;
    if (v.is<picojson::null>()) {
        printf("null\n");
    } else if (v.is<bool>()) {
        std::cout << (v.get<bool>() ? "true" : "false") << '\n';
        return;
    } else if (v.is<double>()) {
        std::cout << v.get<double>() << '\n';
        return;
    } else if (v.is<std::string>()) {
        std::cout << v.get<std::string>() << '\n';
        return;
    } else if (v.is<picojson::array>()) {
        printf("\n");
        int tmpIndent = *indent +1;
        const picojson::array &a = v.get<picojson::array>();
        int index = 0;
        for (picojson::array::const_iterator i = a.begin(); i != a.end(); ++i) {
            print_indent_v( tmpIndent );
            printf("[%d]\n", index++);
            dump_item_v( *i, &tmpIndent );
        }
        return;

    } else if (v.is<picojson::object>()) {
        const picojson::object &o2 = v.get<picojson::object>();
        print_indent_v( *indent );
        dump_obj_v(o2, *indent+1);
        return;
    } else {
        printf("\n");
    }
}

inline void dump_obj_v( const picojson::object &ob, int indent=0)
{
    for (picojson::object::const_iterator i = ob.begin(); i != ob.end(); ++i) {
        dumb_val_type_v( i->second, indent ); std::cout << i->first << ": ";
        dump_item_v(i->second, &indent);
    }
}

inline void dump_mqtt_json_obj(char * buf, int len)
{
    picojson::value v;
    std::string err;

    parse(v, buf, buf+len, &err);
    if (! err.empty() ) {
        throw std::runtime_error("Unable to parse!");
    } else if (v.is<picojson::object>()) {
        // int i = 0;
        std::cout << "\n";
        const picojson::object &o = v.get<picojson::object>();
        dump_obj_v(o);
    }
}
#endif

#elif (DUMP_TYPE==DUMP_TYPE_NONE)
/* ---------------------------- dont parse payload -------------------------- */
inline void dump_mqtt_json_obj(char * buf, int len)
{
    /* nothing */
}

#else
    #error Invalid DUMP_TYPE
#endif /* DUMP_TYPE */

} /* Dump */


#endif /* DUMP_hpp */
