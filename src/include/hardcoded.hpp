#ifndef HARDCODED__hpp
#define HARDCODED__hpp

/* ./gethostip x.x.x.x */
/* Misc */
#define LOCALHOST_HEX 0x7F000001
#define LO_HEX        0x00000000

/*
Test server:
/1J89aIcJMBs/test
mosquitto_sub -h test.mosquitto.org -t /1J89aIcJMBs/test -v
mosquitto_pub -h test.mosquitto.org -t /1J89aIcJMBs/test -m '{"test":1234}'
*/

/* OUI list:
* http://standards-oui.ieee.org/oui.txt
* https://code.wireshark.org/review/gitweb?p=wireshark.git&a=search&h=HEAD&st=grep&s=espressif&sr=1
*/
#define ESPRESSIF_MFG_ID 0x02E5


#endif /* HARDCODED__hpp */
