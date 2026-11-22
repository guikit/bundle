#!/usr/bin/env bash
set -euo pipefail

# If launched from a Linux GUI file manager without a terminal, re-open in a terminal window
if [[ ! -t 0 && ! -t 1 && "${TERM:-dumb}" == "dumb" && -n "${DISPLAY:-}${WAYLAND_DISPLAY:-}" && -z "${GUIKIT_IN_TERM:-}" ]]; then
    export GUIKIT_IN_TERM=1
    for term in x-terminal-emulator gnome-terminal konsole xfce4-terminal mate-terminal kitty alacritty xterm; do
        if command -v "$term" >/dev/null 2>&1; then
            if [[ "$term" == "gnome-terminal" ]]; then
                exec "$term" -- bash -c '"$0" "$@"; echo ""; read -rp "Press Enter to close..."' "$0" "$@"
            else
                exec "$term" -e bash -c '"$0" "$@"; echo ""; read -rp "Press Enter to close..."' "$0" "$@"
            fi
        fi
    done
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
cd "$REPO_ROOT"

CONFIG="${1:-Release}"
if [[ "${CONFIG,,}" == "debug" ]]; then
    CONFIG="Debug"
else
    CONFIG="Release"
fi

BUILD_DIR="build_sdl"

echo "========================================================"
echo " GuiKit SDL Build ($CONFIG) — SDL3 + ThorVG + FreeType"
echo "========================================================"

if [[ ! -f "/usr/include/X11/Xlib.h" || ! -f "/usr/include/GL/gl.h" ]] || ! command -v pkg-config >/dev/null 2>&1; then
    echo "[INFO] Missing X11 / Wayland / OpenGL development headers for SDL3."
    "${SCRIPT_DIR}/install-deps.sh"
fi

NEEDS_CONFIGURE=0
if [[ ! -f "${BUILD_DIR}/Makefile" && ! -f "${BUILD_DIR}/build.ninja" ]]; then
    NEEDS_CONFIGURE=1
elif [[ -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
    CACHED_TYPE="$(grep -E '^CMAKE_BUILD_TYPE:STRING=' "${BUILD_DIR}/CMakeCache.txt" | cut -d= -f2- || true)"
    if [[ -n "$CACHED_TYPE" && "$CACHED_TYPE" != "$CONFIG" ]]; then
        NEEDS_CONFIGURE=1
    fi
fi

if [[ "$NEEDS_CONFIGURE" -eq 1 ]]; then
    echo "[INFO] Configuring CMake SDL3 + ThorVG + FreeType build directory (${BUILD_DIR}, ${CONFIG})..."
    cmake -B "${BUILD_DIR}" -S . \
        -DCMAKE_BUILD_TYPE="$CONFIG" \
        -DGUIKIT_BUNDLE_BUILD_SDL=ON \
        -DGUIKIT_BUNDLE_BUILD_NATIVE=OFF \
        -DGUIKIT_BUNDLE_BUILD_DEMO=ON \
        -DGUIKIT_BUNDLE_BUILD_TESTS=OFF \
        -DGUIKIT_TEXT_USE_FREETYPE=ON
fi

echo "[INFO] Building guikit_demo ($CONFIG)..."
cmake --build "${BUILD_DIR}" --config "$CONFIG" --target guikit_demo --parallel

echo ""
echo "========================================================"
echo " SDL build succeeded (SDL3 + ThorVG + FreeType)!"
echo " Binary: ${BUILD_DIR}/guikit_demo"
echo "========================================================"
