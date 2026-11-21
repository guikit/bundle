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
STAGE_DIR="${REPO_ROOT}/dist/gh-pages"
REMOTE_URL="git@github.com:guikit/demo.git"
SKIP_BUILD=0
USE_NATIVE=1

for arg in "$@"; do
    case "$arg" in
        --skip-build) SKIP_BUILD=1 ;;
        --native)     USE_NATIVE=1 ;;
        --sdl)        USE_NATIVE=0 ;;
        --remote=*)   REMOTE_URL="${arg#*=}" ;;
        *)
            echo "Usage: $0 [--native|--sdl] [--skip-build] [--remote=<git-url>]"
            exit 1
            ;;
    esac
done

if [[ "$USE_NATIVE" -eq 1 ]]; then
    BUILD_DIR="${REPO_ROOT}/build_wasm_native"
    TARGET_NAME="guikit_demo_native"
    BUILD_FLAG="--native"
    STACK_DESC="Emscripten + Native WebGL2 + FreeType (zero SDL3, zero ThorVG)"
else
    BUILD_DIR="${REPO_ROOT}/build_wasm"
    TARGET_NAME="guikit_demo"
    BUILD_FLAG="--sdl"
    STACK_DESC="Emscripten + SDL3 + ThorVG + WebGL2"
fi

echo "==> [1/4] Preparing WebAssembly build (${TARGET_NAME})..."
if [[ "$SKIP_BUILD" -eq 0 ]]; then
    "${SCRIPT_DIR}/web-build.sh" "$BUILD_FLAG"
else
    echo "    Skipping build (--skip-build specified)."
fi

for f in "${BUILD_DIR}/${TARGET_NAME}.html" "${BUILD_DIR}/${TARGET_NAME}.js" "${BUILD_DIR}/${TARGET_NAME}.wasm"; do
    if [[ ! -f "$f" ]]; then
        echo "Error: Required WebAssembly artifact not found: $f" >&2
        exit 1
    fi
done

echo "==> [2/4] Staging static bundle in ${STAGE_DIR}..."
rm -rf "$STAGE_DIR"
mkdir -p "$STAGE_DIR"

cp "${BUILD_DIR}/${TARGET_NAME}.html" "${STAGE_DIR}/index.html"
cp "${BUILD_DIR}/${TARGET_NAME}.html" "${STAGE_DIR}/${TARGET_NAME}.html"
cp "${BUILD_DIR}/${TARGET_NAME}.js"   "${STAGE_DIR}/${TARGET_NAME}.js"
cp "${BUILD_DIR}/${TARGET_NAME}.wasm" "${STAGE_DIR}/${TARGET_NAME}.wasm"
if [[ -f "${BUILD_DIR}/${TARGET_NAME}.data" ]]; then
    cp "${BUILD_DIR}/${TARGET_NAME}.data" "${STAGE_DIR}/${TARGET_NAME}.data"
fi
touch "${STAGE_DIR}/.nojekyll"

SOURCE_COMMIT="$(git -C "$REPO_ROOT" rev-parse --short HEAD 2>/dev/null || echo "unknown")"
UTC_NOW="$(date -u +"%Y-%m-%d %H:%M:%S UTC")"

cat > "${STAGE_DIR}/README.md" <<EOF
# GuiKit — WebAssembly Interactive Demo

Live WebAssembly build of **GuiKit** (\`${TARGET_NAME}\`), compiled with ${STACK_DESC}.

- **Live Demo (GitHub Pages)**: [https://guikit.github.io/demo/](https://guikit.github.io/demo/)
- **Source Repository**: \`guikit/bundle\` (\`@${SOURCE_COMMIT}\`)
- **Last Published**: ${UTC_NOW}
EOF

echo "==> [3/4] Creating deployment commit (source @${SOURCE_COMMIT})..."
pushd "$STAGE_DIR" >/dev/null
git init -b main >/dev/null
git add -A
git commit -m "deploy(wasm): publish ${TARGET_NAME} @${SOURCE_COMMIT} (${UTC_NOW})" >/dev/null

echo "==> [4/4] Pushing to ${REMOTE_URL} (branches: main, gh-pages)..."
git remote add origin "$REMOTE_URL"
git push --force origin HEAD:main HEAD:gh-pages
popd >/dev/null

echo ""
echo "[OK] Published successfully to ${REMOTE_URL}!"
echo "     URL: https://guikit.github.io/demo/"
