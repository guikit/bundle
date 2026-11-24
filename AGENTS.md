# GuiKit Bundle — Agent & Developer Guide (`AGENTS.md`)

This repository (`guikit/bundle`) is the unified workspace aggregating the entire **GuiKit** C++20 ecosystem, its umbrella CMake target (`guikit::all`), and the full 38-screen interactive showcase (`guikit_demo`, Desktop & WebAssembly).

> **Authoritative Core API & Code Style Reference**: Always read and follow [`guikit/AGENTS.md`](guikit/AGENTS.md) for GuiKit UI idioms, layout rules, `keel` zero-STL header purity, naming conventions, and frame-arena lifetimes.

---

## 1. Repository & Submodule Map

| Path | Upstream Repo | CMake Target | Primary Umbrella Header | Scope |
| :--- | :--- | :--- | :--- | :--- |
| `guikit/` | `guikit/guikit` | `guikit::guikit` / `guikit::core` | `"guikit/guikit.hpp"` | Pure C++20 core (0BSD, `keel` only): immediate-mode GUI, layout engine, widgets, panels, `Scene`, `ImageView`, `NullCanvas`, `IPlatformWindow`, `PlatformWindowBackend`, `guikit_headless` |
| `text/` | `guikit/text` | `guikit::text` | `"guikit/text.hpp"` | Standalone FreeType hinted glyph rasterizer & LRU glyph cache shared across CPU & GPU canvases |
| `sdl/` | `guikit/sdl` | `guikit::sdl` (`backend_sdl3`, `backend_thorvg`) | `"guikit/sdl.hpp"` | Cross-platform SDL3 windowing, OpenGL ES2/3 / WebGL2 shader underlays, ThorVG CPU vector rasterizer, `guikit_minimal` |
| `native/` | `guikit/native` | `guikit::native` | `"guikit/native.hpp"` | Lightweight native platform windowing (X11 / Wayland / Win32) and OpenGL ES3 / GL3 2D renderer (zero SDL3, zero ThorVG) |
| `geo/` | `guikit/geo` | `guikit::geo` | `"guikit/geo.hpp"` | Multi-CRS geodesy (`MapGrid`), GeoJSON I/O, S-52/ENC GIS engine, OSM tiles, `guikit::geo::MapView` |
| `signal/` | `guikit/signal` | `guikit::signal` | `"guikit/signal.hpp"` | `guikit::signal::SignalView`, `SonarView`, `WaterfallView` (GLSL underlays + CPU fallback) |
| `cockpit/` | `guikit/cockpit` | `guikit::cockpit` | `"guikit/cockpit.hpp"` | `guikit::cockpit::AttitudeView` (PFD), `CarClusterView` (automotive cockpit) |
| `diagram/` | `guikit/diagram` | `guikit::diagram` | `"guikit/diagram.hpp"` | `guikit::diagram::BpmnEditor` (BPMN 2.0 modeler & token simulator) |
| `demo/` | *(in `bundle`)* | `guikit_demo` | — | Unified 38-screen interactive showcase, `demo/assets/images/lena.png`, `demo/shell.html` |
| `scripts/` | *(in `bundle`)* | — | — | Desktop build/run and WebAssembly build/preview/GitHub Pages publish scripts |

---

## 2. Core Golden Rules (Summary of [`guikit/AGENTS.md`](guikit/AGENTS.md))

1. **Zero-STL Public Headers (`keel` only)**: All public headers across `guikit/`, `geo/`, `signal/`, `cockpit/`, and `diagram/` are checked by `guikit_enforce_guidelines` (`*_header_purity_scan` in CTest). Use `keel::*` types (`keel::i32`, `keel::usize`, `keel::StringRef`, `keel::ArrayRef`, `keel::Optional`, `keel::SmallArray`, `"keel/math.h"`), never STL containers or `<cmath>`/`<cstdint>` in public headers.
2. **Unified Free-Function Widget API & UI Idioms**: All core widgets, layouts, and panels are free functions taking `guikit::Context& ui` as the first argument (`guikit::Button(ui, ...)`, `guikit::Row(ui, ...)`, `guikit::Card(ui, ...)`, `guikit::Element(ui, ...)`, `guikit::Heading(ui, ...)`, matching `guikit::geo::MapView(ui, ...)`, `guikit::signal::SignalView(ui, ...)`, `guikit::cockpit::AttitudeView(ui, ...)`, `guikit::diagram::BpmnEditor(ui, ...)`). Never add widget forwarding methods onto `guikit::Context`. Use `guikit::MainWindow` (no raw SDL/GL loops), flat `.size` (`UiSize`) / `.width` / `.height` (`AxisSize`), `ui.Zone(...)` for theme/scale/density, `ui.IdScope(...)` for dynamic list stability, `ui.Format(...)` for zero-heap per-frame strings, and `StateColor` / `ui.GetThemePalette()` instead of hardcoded colors.
3. **Header Hygiene**: `guikit/context.hpp` is a pure engine header with zero widget/panel/layout includes; include individual widget headers or `"guikit/guikit.hpp"` / `"guikit/core.hpp"` for full umbrella access.
4. **Demo Assets**: Core library assets live in `guikit/assets/`; module-specific assets live in `<module>/assets/` (e.g. `geo/assets/geo/demo_bay.geojson`); showcase-only assets live in `demo/assets/` and are embedded via `cmake/GenerateDemoAssets.cmake` (`guikit::demo::RegisterDemoEmbeddedAssets()`).

---

## 3. Build, Test & Deploy Commands

Run from the `bundle/` root:

```bash
# Configure & build everything (all 5 modules, guikit_demo, guikit_minimal, all tests)
cmake -S . -B build
cmake --build build --parallel

# Run all 14 ecosystem CTest suites (unit tests + header purity scans)
ctest --test-dir build --output-on-failure --parallel

# Launch native showcase (zero SDL3 / ThorVG / FreeType)
./scripts/linux/native-run.sh         # Linux
# scripts\windows\native-run.cmd      # Windows

# Launch SDL3 + ThorVG + FreeType showcase
./scripts/linux/sdl-run.sh            # Linux
# scripts\windows\sdl-run.cmd         # Windows

# Build / preview / publish WebAssembly demo (https://guikit.github.io/demo/)
./scripts/linux/web-build.sh
./scripts/linux/web-run.sh
./scripts/linux/web-publish.sh
```

---

## 4. Submodule Git Workflow

Because each module is an independent GitHub repository under `guikit/*`:
1. Before editing inside a submodule (`guikit`, `geo`, `signal`, `cockpit`, `diagram`), ensure it is on branch `main` (not detached `HEAD`): `git -C <module> checkout main`.
2. Verify all builds and `ctest` pass from `bundle/build` with **0 warnings, 0 errors**.
3. **Commit & push modified submodules first**, then commit & push the updated submodule gitlinks (and any `demo/` / `scripts/` changes) in `bundle` last so remote `bundle` never points to unpushed commits.
