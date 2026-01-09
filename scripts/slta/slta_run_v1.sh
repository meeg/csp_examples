#!/usr/bin/env bash
set -euo pipefail

EXPOSURE="${EXPOSURE:-10}"
NSAMP="${NSAMP:-1}"
TOTNCOL="${TOTNCOL:-1200}"
TOTNROW="${TOTNROW:-1350}"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --nsamp|--NSAMP)
      NSAMP="$2"; shift 2 ;;
    --exposure|--EXPOSURE)
      EXPOSURE="$2"; shift 2 ;;
    --clear|--CLEAR)
      CLEAR="$2"; shift 2 ;;
    --runname|--RUNNAME)
      RUNNAME="$2"; shift 2 ;;
    --port|--PORT)
      PORT="$2"; shift 2 ;;
    -h|--help)
      cat <<EOF
Usage: $0 [options]

Options:
  --exposure, --exptime <sec>   Exposure time in seconds (sets EXPOSURE)
  --nsamp <N>                   Number of samples (sets NSAMP)
  --clear <sec>                 Clear dwell time in seconds (sets CLEAR)
  --runname <name>              Run name prefix (sets RUNNAME)
  --port <port>                 LTA daemon port (sets PORT)
  -h, --help                    Show this help
EOF
      exit 0
      ;;
    *)
      echo "Unknown argument: $1" >&2
      exit 1
      ;;
  esac
done


# Defaults 
PORT="${PORT:-8888}"
CLEAR="${CLEAR:-30}"
INITSCRIPT="${INITSCRIPT:-./slta_init_v1.sh}"
EXPOSESEQ="${EXPOSESEQ:-./slta_sequencer_image_v1.xml}"
CLEARSEQ="${CLEARSEQ:-./slta_sequencer_clear_v1.xml}"
RUNNAME="${RUNNAME:-darkness}"

export daemonport="$PORT"
lta() { ./lta.sh "$PORT" "$@"; }

source "$INITSCRIPT"

# Helper
apply_params() {
  lta NSAMP "$NSAMP"
  lta EXPOSURE "$EXPOSURE"
  lta NCOL "$TOTNCOL"
  lta NROW "$TOTNROW"
}

# Run
lta sseq "$EXPOSESEQ"
apply_params

if (( CLEAR > 0 )); then
  lta sseq "$CLEARSEQ"
  apply_params

  lta NROW 1000000000
  lta startseq
  sleep "$CLEAR"
  lta readoff

  lta sseq "$EXPOSESEQ"
  apply_params
fi

STAMP="$(date -u +%Y%m%dT%H%M%SZ)"
lta name "$IMGFOLDER/${RUNNAME}_${STAMP}_EXPOSURE${EXPOSURE}_NSAMP${NSAMP}_CLEAR${CLEAR}_"
lta read
echo "done"

