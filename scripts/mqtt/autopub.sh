#!/bin/bash
ARG1="$1"

source creds.sh;

CMD_BASE=('mosquitto_pub' '-V' 'mqttv311');

if ! [[ -z $MQTT_USER ]] && ! [[ -z $MQTT_PASS ]]; then
  CMD_TO_RUN=("${CMD_BASE[@]}");
  CMD_TO_RUN+=('-h' "$MQTT_HOST" '-u' "$MQTT_USER" '-P' "$MQTT_PASS" '-t' "$MQTT_TOPIC");
else
  CMD_TO_RUN=("${CMD_BASE[@]}");
  CMD_TO_RUN+=('-h' "$MQTT_HOST" '-t' "$MQTT_TOPIC");
fi

PUB_DELAY="2"

do_pub0() {
  echo "do_pub0";
  local tmpcmd=( "${CMD_TO_RUN[@]}" '-m' "\"$1 - do_pub0\"" '--qos' "0");
  "${tmpcmd[@]}";
}

do_pub1() {
  echo "do_pub1";
  local tmpcmd=( "${CMD_TO_RUN[@]}" '-m' "\"$1 - do_pub1\"" '--qos' "1");
  "${tmpcmd[@]}";
}

do_pub2() {
  echo "do_pub2";
  local tmpcmd=( "${CMD_TO_RUN[@]}" '-m' "\"$1 - do_pub2\"" '--qos' "2");
  "${tmpcmd[@]}";
}

main() {
local i=0;
while true; do
  sleep "$PUB_DELAY";
  local msg='# testing 1234';

  ((i++));

case "$ARG1" in
  "0") do_pub0 "$msg";
  ;;
  "1") do_pub1 "$msg";
  ;;
  "2") do_pub2 "$msg";
  ;;
  *) echo "invalid";
  exit 1;
  ;;
esac

done

}



main;




exit 0;



#### eof
