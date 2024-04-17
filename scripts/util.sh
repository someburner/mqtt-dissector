#################################################################################
#                                 MINI VERSION                                  #
#################################################################################
_pmod=0;
paint() { local c=1; case "$1" in "nrm") c=0;; "gry") c=30;; "red") c=31;; "grn") c=32;; "ylw") c=33;; "blu") c=34;; "pur") c=35;; "aqu") c=36;; "wht") c=1;; esac; printf "\\033[$_pmod;$c""m$2""\033[m"; };
lpaint() { local c=1; case "$1" in "nrm") c=0;; "gry") c=90;; "red") c=91;; "grn") c=92;; "ylw") c=93;; "blu") c=94;; "pur") c=95;; "aqua") c=96;; "wht") c=97;; *);; esac; printf "\\033[$_pmod;$c""m$2""\033[m"; };
paintln() { paint "$1" "$2\n"; }; lpaintln() { lpaint "$1" "$2\n"; };
painthi() { _pmod=7; paint "$1" "$2"; }; painthiln() { _pmod=7; paintln "$1" "$2"; };
lpainthi() { _pmod=7; lpaint "$1" "$2"; }; lpainthiln() { _pmod=7; lpaintln "$1" "$2"; };
do_warn() { paint 'ylw' '[WARN]'; printf ' %s\n' "$1"; }; do_error() { lpaint 'red' '[ERROR]'; printf ' %s\n' "$1"; };
do_okay() { lpaint 'grn' '[OKAY]'; printf ' %s\n' "$1"; }; do_debug() { lpaint 'blu' '[DEBUG]'; printf " $1\n"; };
should_exit() { if ! [[ $1 -eq 0 ]]; then paint 'red' '[FATAL]'; printf " Got $1"; exit $1; fi }
should_warn() { if ! [[ $1 -eq 0 ]]; then paint 'ylw' '[WARN]'; printf "Got: $1"; return 1; else return 0; fi; }
#################################################################################
