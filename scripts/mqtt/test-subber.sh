#!/bin/bash

LOGFILE="logs/test.log";
NOTICE="logs/notice";

source creds.sh;

CMD_BASE=( 'mosquitto_sub' '-V' 'mqttv311' );

gen_cmd() {
  CMD_TO_RUN=( "${CMD_BASE[@]}" );

if ! [[ -z $MQTT_ID ]]; then
  CMD_BASE=( "${CMD_BASE[@]}" '-I' "\"$MQTT_ID\"" );
fi
if ! [[ -z $MQTT_USER ]] && ! [[ -z $MQTT_PASS ]]; then
  CMD_TO_RUN=( "${CMD_BASE[@]}" '-h' "$MQTT_HOST" '-u' "$MQTT_USER" '-P' "$MQTT_PASS" '--qos' "$MQTT_QOS" '-t' "$MQTT_TOPIC");
else
  CMD_TO_RUN=( "${CMD_BASE[@]}" '-h' "$MQTT_HOST" '-t' "$MQTT_TOPIC" '-q' "$MQTT_QOS");
fi

if ! [[ -z $WILL_PAYLOAD ]] && ! [[ -z $WILL_QOS ]] && ! [[ -z $WILL_TOPIC ]]; then
  CMD_TO_RUN=( "${CMD_TO_RUN[@]}" '--will-payload' "$WILL_PAYLOAD" '--will-qos' "$WILL_QOS" '--will-topic' "$WILL_TOPIC");
  if ! [[ -z $WILL_RETAIN ]]; then
    CMD_TO_RUN=( "${CMD_TO_RUN[@]}" '--will-retain');
  fi
fi
} # gen_cmd

CMD_TO_RUN=();


log_rotate() {
  mkdir -p logs;
  if [[ -f $LOGFILE ]]; then
    cp $LOGFILE "$LOGFILE.$(date '+%s').save";
    echo "Build init: $(date) [ $(date '+%s') ]" > $LOGFILE;
  fi
} # log_rotate

kill_stuff() {
  local THIS=$BASHPID
  TRASH=$(pgrep 'test-subber');
  for i in $TRASH; do
    case "$i" in
      "$THIS") continue;;
      *) echo "Killing $i.."; kill $i;;
    esac
  done
} # kill_stuff

finally() {
  echo "$(tail $LOGFILE -n 50)" >> $NOTICE;
} # finally

####################

print_help() { echo 'Args:
 * 0 - test sub (Qos 0)
 * 1 - test sub (Qos 1)
 * 2 - test sub (Qos 2)
 * p - pgrep test-subber
 * kill - kill any instances of this script

';
} # print_help

MQTT_QOS='3';

case "$1" in
  ######### valid subs #########
  "0") MQTT_QOS='0'; gen_cmd;;
  "1") MQTT_QOS='1'; gen_cmd;;
  "2") MQTT_QOS='2'; gen_cmd;;
  ######### Misc opts #########
  "p") echo; echo "$(ps -aux | grep test-subber)"; echo; exit 0;;
  "kill") echo "killing procs";
    kill_stuff;
    exit 0;
  ;;
  ########## invalid ###########
  *) echo "invalid"; print_help;
    exit 1;
  ;;
esac

#kill_stuff;
#log_rotate;
# { coproc maketee { tee -a $LOGFILE ;} >&3 ;} 3>&1
# cmd=("${CMD_TO_RUN[*]}" ">&${maketee[1]}");

printf "\n\t ## ${CMD_TO_RUN[*]} ##\n\n";
eval "${CMD_TO_RUN[@]}";
exit $?;
##eval '( $cmd 2>&1 ; 1>/dev/null; echo "Stopped" >> $NOTICE; finally; )' &>$LOGFILE &disown;


####################
