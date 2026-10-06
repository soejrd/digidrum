#!/bin/zsh
# Double-click this file in Finder to open Digidrum TRX with digihealth.
set -u

here="${0:A:h}"
emu="$here/../digiemu"
python="$emu/.venv/bin/python"
firmware="$here/out/Digitakt_OS1.53_DIGIDRUM_TRX_OPT_HEALTH.syx"
data="$here/out/trx-emulator"

if [[ ! -x "$python" || ! -f "$firmware" ]]; then
  print 'Digidrum needs the local Digiemu Python and the built firmware.'
  print "Expected firmware: $firmware"
  print 'See README.md for the build commands.'
  read '?Press Return to close...'
  exit 1
fi

cd "$emu" || exit 1
"$python" - "$firmware" "$data" <<'PY'
import subprocess
import sys

from emu import portable

firmware, data = sys.argv[1:]
try:
    plan = portable.plan_add(firmware, data)
except Exception as exc:
    print(f"Could not prepare Digidrum: {exc}", flush=True)
    raise SystemExit(1)

print("Preparing Digidrum in Digiemu (the first launch takes longer)...", flush=True)
result = portable.add_firmware(firmware, yes=True, home=data)
if result == portable.EXIT_NOT_READY:
    result = portable.rebuild_firmware(plan.release.slug, home=data)
if result:
    print(f"Digiemu setup stopped with code {result}.", flush=True)
    raise SystemExit(result)

print("Opening the Digitakt panel...", flush=True)
raise SystemExit(subprocess.call(
    portable.worker_command("panel", plan.fwdir),
    cwd=portable.REPO,
    env=portable.child_env(),
))
PY
result=$?
if (( result != 0 )); then
  print "Digidrum did not open (exit $result)."
  print "Emulator data and logs: $data"
  read '?Press Return to close...'
fi
exit $result
