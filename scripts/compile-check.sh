#!/usr/bin/env bash
# Compile every example (and the test sketches) for every VTLabs board / option set.
# Usage: scripts/compile-check.sh [extra arduino-cli args, e.g. --config-file x.yaml]
#
# Prerequisites (done once):
#   arduino-cli core install esp32:esp32@3.3.12
#   VTLabs platform visible to arduino-cli, either installed via Boards Manager or
#   linked into <sketchbook>/hardware/vtlabs  (see docs/02-stage2-board-definition.md)
set -uo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CLI_ARGS=("$@")

# FQBN list: default options plus one non-default combination per board.
FQBNS=(
  "vtlabs:esp32:vtlabs_iot_esp32_r1"
  "vtlabs:esp32:vtlabs_iot_esp32_r1:PartitionScheme=huge_app,CPUFreq=80,DebugLevel=verbose"
)

pass=0; fail=0; failed=()
while IFS= read -r -d '' ino; do
  sketch_dir="$(dirname "$ino")"
  for fqbn in "${FQBNS[@]}"; do
    if arduino-cli "${CLI_ARGS[@]}" compile --fqbn "$fqbn" "$sketch_dir" >/tmp/vtlabs-compile.log 2>&1; then
      echo "PASS  $fqbn  ${sketch_dir#$ROOT/}"
      pass=$((pass+1))
    else
      echo "FAIL  $fqbn  ${sketch_dir#$ROOT/}"
      grep -E "error|Error" /tmp/vtlabs-compile.log | grep -v "discovery" | head -5 | sed 's/^/      /'
      fail=$((fail+1)); failed+=("$fqbn $sketch_dir")
    fi
  done
done < <(find "$ROOT/examples" "$ROOT/tests/sketches" -name '*.ino' -print0 | sort -z)

echo "----"
echo "passed: $pass  failed: $fail"
[ "$fail" -eq 0 ]
