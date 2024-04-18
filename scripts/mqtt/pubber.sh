#!/bin/bash
ARG1="$1"

source creds.sh;

CMD_BASE='mosquitto_pub -V mqttv311';
PUB_REPEAT="9999";
PUB_DELAY="2";

# CID0="client0";
CID0="client0-$(date +%s)";
CID1="client1-$(date +%s)";
CID2="client2-$(date +%s)";

if [[ -z $MQTT_TOPIC ]]; then
  echo "MQTT_TOPIC unset";
  exit 1;
fi

if ! [[ -z $MQTT_USER ]] && ! [[ -z $MQTT_PASS ]]; then
  CMD_TO_RUN="$CMD_BASE -h $MQTT_HOST -u $MQTT_USER -P $MQTT_PASS -t \"$MQTT_TOPIC\"";
else
  CMD_TO_RUN="$CMD_BASE -h $MQTT_HOST -t \"$MQTT_TOPIC\"";
fi

! [[ -z $COUNT ]] && PUB_REPEAT=$COUNT;
! [[ -z $INTERVAL ]] && PUB_DELAY=$INTERVAL;

CMD_TO_RUN="$CMD_TO_RUN --repeat $PUB_REPEAT --repeat-delay $PUB_DELAY";

do_pub0() {
  echo "do_pub0";
  local tmpcmd="$CMD_TO_RUN -i $CID0 -m pub0_cid0";
  eval "$tmpcmd";
}

do_pub1() {
  echo "do_pub1";
  local tmpcmd="$CMD_TO_RUN -i $CID1 -m pub1_cid1";
  eval "$tmpcmd";
}

do_pub2() {
  echo "do_pub2";
  local tmpcmd="$CMD_TO_RUN -i $CID2 -m pub2_cid2";
  eval "$tmpcmd";
}

main() {
case "$ARG1" in
  "0") do_pub0;
  ;;
  "1") do_pub1;
  ;;
  "2") do_pub2;
  ;;
  *) echo "invalid";
  exit 1;
  ;;
esac
}

main;

exit 0;
