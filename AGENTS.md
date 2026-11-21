# GuiKit Bundle — Agent & Developer Guide (`AGENTS.md`)

This repository (`guikit/bundle`) is the unified workspace aggregating the entire **GuiKit** C++20 ecosystem, its umbrella CMake target (`guikit::all`), and the full 38-screen interactive showcase (`guikit_demo`, Desktop & WebAssembly).

> **Authoritative Core API & Code Style Reference**: Always read and follow [`guikit/AGENTS.md`](guikit/AGENTS.md) for GuiKit UI idioms, layout rules, `keel` zero-STL header purity, naming conventions, and frame-arena lifetimes.

---

## 1. Repository & Submodule Map

| Path | Upstream Repo | CMake Target | Primary Umbrella Header | Scope |
| :--- | :--- | :--- | :--- | :--- |
| `guikit/` | `guikit/guikit` | `guikit::guikit` / `guikit::core` | `"guikit/guikit.hpp"` | Core immediate-mode GUI, layout engine, widgets, panels, `Scene`, `ImageView`, backends, `guikit_minimal` |
| `geo/` | `guikit/geo` | `guikit::geo` | `"guikit/geo.hpp"` | Multi-CRS geodesy (`MapGrid`), GeoJSON I/O, S-52/ENC GIS engine, OSM tiles, `guikit::geo::MapView` |
| `signal/` | `guikit/signal` | `guikit::signal` | `"guikit/signal.hpp"` | `guikit::signal::SignalView`, `SonarView`, `WaterfallView` (GLSL underlays + CPU fallback) |
| `cockpit/` | `guikit/cockpit` | `guikit::cockpit` | `"guikit/cockpit.hpp"` | `guikit::cockpit::AttitudeView` (PFD), `CarClusterView` (automotive cockpit) |
| `diagram/` | `guikit/diagram` | `guikit::diagram` | `"guikit/diagram.hpp"` | `guikit::diagram::BpmnEditor` (BPMN 2.0 modeler & token simulator) |
| `demo/` | *(in `bundle`)* | `guikit_demo` | — | Unified 38-screen interactive showcase, `demo/assets/images/lena.png`, `demo/shell.html` |
| `scripts/` | *(in `bundle`)* | — | — | Desktop build/run and WebAssembly build/preview/GitHub Pages publish scripts |

---

## 2. Core Golden Rules (Summary of [`guikit/AGENTS.md`](guikit/AGENTS.md))

1. **Zero-STL Public Headers (`keel` only)**: All public headers across `guikit/`, `geo/`, `signal/`, `cockpit/`, and `diagram/` are checked by `guikit_enforce_guidelines` (`*_header_purity_scan` in CTest). Use `keel::*` types (`keel::i32`, `keel::usize`, `keel::StringRef`, `keel::ArrayRef`, `keel::Optional`, `keel::SmallArray`, `"keel/math.h"`), never STL containers or `<cmath>`/`<cstdint>` in public headers.
2. **UI Idioms**: Use `guikit::MainWindow` (no raw SDL/GL loops), `ui.Element(...)` + `ui.Heading(...)` (no manual label scaffolding), flat `.size` (`UiSize`) / `.width` / `.height` (`AxisSize`), `ui.Zone(...)` for theme/scale/density, `ui.Format(...)` for zero-heap per-frame strings, and `StateColor` / `ui.GetThemePalette()` instead of hardcoded colors.
3. **Domain Module API Style**: Companion modules expose free functions taking `guikit::Context& ui` as the first argument (`guikit::geo::MapView(ui, ...)`, `guikit::signal::SignalView(ui, ...)`, `guikit::cockpit::AttitudeView(ui, ...)`, `guikit::diagram::BpmnEditor(ui, ...)`). Never add domain-specific widget methods back onto `guikit::Context`.
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

# Launch full interactive desktop showcase
./scripts/linux/desktop-run.sh        # Linux
# scripts\windows\desktop-run.cmd     # Windows

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
