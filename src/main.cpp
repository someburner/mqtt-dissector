/* --- /precompiled --- */
// #include "include/stdafx.h"
#include <cstdint>
#include <iostream>
#include <unistd.h> // getuid

/* project includes */
#include "options.hpp"
#include "debug.h"

/* project includes */
#include "dissect.hpp"

#include "include/clipp.h"
#include "include/version.hpp"

int main(int argc, char* argv[]) {
    if (getuid() != 0) {
        std::cout << "Must be root." << std::endl;
        exit(1);
    }

    bool print_version = false;
    bool daemonize = false;
    bool show_ifs = false;
    bool show_help = false;
    bool en_logging = false;
    bool en_dns = false;
    bool dis_partials = false;
    bool en_mac_filter = false;
    bool en_ip_filter = false;
    bool dump_payloads = false;
    std::string iface = "invalid";
    static std::string pidfile;
    static std::string logpath;
    static std::string mac_filter = "";
    static std::string ip_filter = "";
    std::vector<std::string> wrong;

    auto cli = (
        clipp::option("-i",   "--iface")  & clipp::value("interface", iface),
        clipp::option("-d",   "--daemonize").set(daemonize).doc("run as daemon"),
        clipp::option("-l",   "--logpath").set(en_logging).doc("/path/to/logs") & clipp::value("logpath", logpath),
        clipp::option("-dns", "--en-dns").set(en_dns).doc("Enable DNS capture"),
        clipp::option("-dp",  "--disable-partials").set(dis_partials).doc("ignore partial streams"),
        clipp::option("-fm",  "--filter-mac").set(en_mac_filter).doc("MAC filter") & clipp::value("MAC", mac_filter),
        clipp::option("-fi",  "--filter-ip").set(en_ip_filter).doc("IP filter") & clipp::value("IP", ip_filter),
        clipp::option("-p",   "--payload").set(dump_payloads).doc("Dump payloads"),
        // clipp::option("-pid", "--pidfile") & clipp::value("pid file location (daemon)", pidfile),
        clipp::option("-v",   "--version").set(print_version).doc("print version and exit"),
        clipp::option("-h",   "--help").set(show_help).doc("Print help"),
        clipp::option("-s",   "--show").set(show_ifs).doc("List available interfaces and exit"),
        /* catch-all for errors */
        clipp::any_other(wrong)
    );

    auto cli_fmt = clipp::doc_formatting{}
        .paragraph_spacing(0)
        .doc_column(24)
        .start_column(4);

    if ( (argc < 2) || !clipp::parse(argc, argv, cli) || !wrong.empty() ) {
        if(!wrong.empty()) {
            std::cout << "Unknown command line arguments:\n";
            for(const auto& a : wrong) std::cout << "'" << a << "'\n";
            std::cout << "For usage, run with -h\n\n";
            return 1;
        }
        show_help = true;
    }
    // const char * test = APP_NAME;

    if (show_help) {
        std::cout << make_man_page(cli, argv[0], cli_fmt)
            .prepend_section("DESCRIPTION", "    MQTT Dissector")
            .append_section("EXAMPLES",
                "\t./mqtt-dissector -s\n"
                "\t./mqtt-dissector -i wl0.1\n"
                "\t./mqtt-dissector -i wl0.1 -fm 5c:cf:7f:3a:a1:97\n"
                "\t./mqtt-dissector -i lo -fi 127.0.0.1\n"
            );
        return 1;
    }
    if (print_version) {
        std::cout << "Version: " << get_version() << "\n";
        std::cout << "   [ libpcap-" << get_pcap_version() << " ]\n";
        std::cout << "   [ libtins-" << get_tins_version() << " ]\n";
        return 0;
    }
    if (show_ifs) {
        dissect_help();
        return 0;
    }

    logdebug("Logging init");
    logdebug("Interface provided: %s", iface.c_str() );
    /* ---------------------------------------------------------------------- */

    FilterList flist;
    if (en_mac_filter) {
        Filter_t * fmac = new Filter_t{
            FILTER_MAC_T,
            mac_filter
        };
        flist.push_back(fmac);
    }
    if (en_ip_filter) {
        Filter_t * fip = new Filter_t{
            FILTER_IP_T,
            ip_filter
        };
        flist.push_back(fip);
    }

    /* ---------------------------------------------------------------------- */

    if (strstr("any", iface.c_str()) != NULL) {
        iface = "any";
    } else if ( !validate_iface(iface) ) {
        exit(1);
    }

    try {
        logdebug("dissect begin..");
        int ret = dissect(iface.c_str(), flist, en_dns, dump_payloads, !dis_partials);
    }
    catch (std::exception& ex) {
        logerror("Error: %s", ex.what() );
    }
}













/* EOF */
