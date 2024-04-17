#!/mmc/bin/bash
ARGS=("$@");
ARGC=$#;
_TOPDIR=$(dirname `realpath $0`);

source /mmc/usr/utils.sh

################# Globals #################
CMD=('');
CLEANUP_CMD=('');
KEY_ARR=();
ASSIGN_VAR="";
###########################################
ARG_HELP_ARR=(
	'set' 'Set tcconfig'
	'del ' 'Del tc'
	'show' 'show filters'
	'test' 'debugging'
);

###########################################


main() {
case "${ARGS[@]::1}" in
"test" ) echo "test stuff";
;;
"set") echo "set tc..."; tc_set "wl0.1" "500" "50" "normal";
;;
"del") tc_del "wl0.1";
;;
"show") tc_show "wl0.1";
;;
* ) echo "You need to pass an argument. Eg:";
# paint 'wht' '\t./doit.sh '; paint 'ylw' 'arg1 arg2'; paint 'wht' ' .. '; paintln 'ylw' 'argN';
printHelp; exit 1;
;;
esac
}

##################################################################

print_cfg_item() {
paint 'nrm' "+$1"; paint 'nrm' ': '; paintln 'wht' "$2";
}


##################################################################

#tcset --device eth0 --delay 10 --delay-distro 2  --loss 0.01 --rate 0.25M --network 192.168.0.10 --port 8080

#tc qdisc show dev wl0.1
#tc qdisc del dev wl0.1 root

tc_set() {
	local _if="$1";
	local _delay_ms="$2"; _delay_ms+="ms";
	local _variance_ms="$3"; _variance_ms+="ms";
	local _method="$4";
	echo "Clearing previous..";

	tc qdisc del dev wl0.1 root
	tc qdisc del dev wl0.1 root stab pfifo

	paint 'aqu' '\n\t >> Adding filters <<\n\n';
	print_cfg_item "ifname" "$_if";
	print_cfg_item "delay" "$_delay_ms";
	print_cfg_item "variance" "+/- $_variance_ms";
	print_cfg_item "distro" "$_method";
	tc qdisc add dev wl0.1 root handle 11a3: htb default 1
	tc class add dev wl0.1 parent 11a3: classid 11a3:1 htb rate 32000000.0kbit
	tc class add dev wl0.1 parent 11a3: classid 11a3:2 htb rate 32000000.0Kbit ceil 32000000.0Kbit
	tc qdisc add dev wl0.1 parent 11a3:2 handle 1223: netem delay "$_delay_ms" "$_variance_ms" distribution "$_method";
#	tc filter add dev wl0.1 protocol ip parent 11a3: prio 2 u32 match ip dst 0.0.0.0/0 match ip src 0.0.0.0/0 match ip dport 1883 0xffff flowid 11a3:2
	tc filter add dev wl0.1 protocol ip parent 11a3: prio 2 u32 match ip dst 0.0.0.0/0 match ip src 0.0.0.0/0 match ip sport 1883 0xffff flowid 11a3:2
	return 0;
}

tc_del() {
	paint 'aqu' "\n\t >> Delete qdiscs for device: $1 <<\n\n";
	tc qdisc del dev "$1" root
	tc qdisc del dev "$1" root stab pfifo
}


tc_show() {
	paint 'aqu' "\n\t >> Show device: $1 <<\n\n";
	tc qdisc show dev "$1"
}


printHelp() {
printf "\nAvailable args:\n";
local ct=${#ARG_HELP_ARR[@]}; local i=0; local tmp1=''; local tmp2='';
for ((i=0; i<ct; i++)); do tmp1="${ARG_HELP_ARR[i]}"; ((i++)); tmp2="${ARG_HELP_ARR[((i))]}";
printf ' * '; paint 'ylw' "$tmp1"; printf " - %s\n" "$tmp2";
done
echo ""
}


main;
##################################################################
echo; echo;


exit 0;
