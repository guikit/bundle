#!/usr/bin/env bash
set -euo pipefail

# If launched from a Linux GUI file manager without a terminal, re-open in a terminal window
if [[ ! -t 0 && ! -t 1 && "${TERM:-dumb}" == "dumb" && -n "${DISPLAY:-}${WAYLAND_DISPLAY:-}" && -z "${GUIKIT_IN_TERM:-}" ]]; then
    export GUIKIT_IN_TERM=1
    for term in x-terminal-emulator gnome-terminal konsole xfce4-terminal mate-terminal kitty alacritty xterm; do
        if command -v "$term" >/dev/null 2>&1; then
            if [[ "$term" == "gnome-terminal" ]]; then
                exec "$term" -- bash -c '"$0" "$@"; status=$?; if [ $status -ne 0 ]; then echo ""; read -rp "Press Enter to close..."; fi; exit $status' "$0" "$@"
            else
                exec "$term" -e bash -c '"$0" "$@"; status=$?; if [ $status -ne 0 ]; then echo ""; read -rp "Press Enter to close..."; fi; exit $status' "$0" "$@"
            fi
        fi
    done
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
cd "$REPO_ROOT"

CONFIG="Release"
if [[ $# -gt 0 ]]; then
    if [[ "${1,,}" == "debug" ]]; then
        CONFIG="Debug"
        shift
    elif [[ "${1,,}" == "release" ]]; then
        CONFIG="Release"
        shift
    fi
fi

BUILD_DIR="build_native"
GUIKIT_BIN=""
for candidate in "${BUILD_DIR}/guikit_demo_native" "${BUILD_DIR}/${CONFIG}/guikit_demo_native" "${BUILD_DIR}/Release/guikit_demo_native" "${BUILD_DIR}/Debug/guikit_demo_native"; do
    if [[ -f "$candidate" ]]; then
        GUIKIT_BIN="$candidate"
        break
    fi
done

if [[ -z "$GUIKIT_BIN" ]]; then
    echo "[guikit] guikit_demo_native binary not found. Building ${CONFIG} (native-only, no SDL3/ThorVG) first..."
    "${SCRIPT_DIR}/native-build.sh" "$CONFIG"
    for candidate in "${BUILD_DIR}/guikit_demo_native" "${BUILD_DIR}/${CONFIG}/guikit_demo_native"; do
        if [[ -f "$candidate" ]]; then
            GUIKIT_BIN="$candidate"
            break
        fi
    done
fi

if [[ -z "$GUIKIT_BIN" ]]; then
    echo "[ERROR] Build failed, guikit_demo_native binary not found." >&2
    exit 1
fi

echo "[guikit] Launching Native Desktop Demo (zero SDL3 / zero ThorVG, CSD HeaderBar): $GUIKIT_BIN --custom-header $*"
exec "$GUIKIT_BIN" --custom-header "$@"
