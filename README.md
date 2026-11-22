# guikit/bundle

Unified workspace, umbrella CMake target (`guikit::all`), and full 38-screen interactive showcase (`guikit_demo`) for the **GuiKit** C++20 ecosystem (Native Desktop & WebAssembly).

## 👉 [Live Interactive Demo (WebAssembly): https://guikit.github.io/demo/](https://guikit.github.io/demo/)

| **Workspace & Rich Markdown Engine** | **S-52 / ENC Marine GIS Vector Chart** |
| :---: | :---: |
| ![Workspace & Rich Markdown](https://raw.githubusercontent.com/guikit/guikit/main/docs/screenshots/markdown_workspace.png) | ![Marine GIS Chart](https://raw.githubusercontent.com/guikit/geo/main/docs/screenshots/gis_chart.png) |
| **PPI 360° Sonar Radar (`SonarView`)** | **Waveform + STFT Spectrogram (`SignalView`)** |
| ![Sonar Radar](https://raw.githubusercontent.com/guikit/signal/main/docs/screenshots/sonar_radar.png) | ![Signal View](https://raw.githubusercontent.com/guikit/signal/main/docs/screenshots/signal_view.png) |
| **RTSA Spectral Waterfall (`WaterfallView`)** | **Avionics Primary Flight Display (`AttitudeView`)** |
| ![Spectral Waterfall](https://raw.githubusercontent.com/guikit/signal/main/docs/screenshots/waterfall_view.png) | ![Attitude PFD](https://raw.githubusercontent.com/guikit/cockpit/main/docs/screenshots/attitude_pfd.png) |
| **Automotive Cockpit & ADAS (`CarClusterView`)** | **BPMN 2.0 Modeler & Token Sim (`BpmnEditor`)** |
| ![Car Cluster](https://raw.githubusercontent.com/guikit/cockpit/main/docs/screenshots/car_cluster.png) | ![BPMN 2.0 Modeler](https://raw.githubusercontent.com/guikit/diagram/main/docs/screenshots/bpmn_modeler.png) |

---

## Included Repositories

| Module | CMake Target | Description |
| :--- | :--- | :--- |
| [`guikit`](https://github.com/guikit/guikit) | `guikit::guikit` | Core immediate-mode vector GUI library, layout engine, widgets, panels, `Scene`, `ImageView`, and rendering/windowing backends |
| [`geo`](https://github.com/guikit/geo) | `guikit::geo` | Cartography, 7 geodetic projections, GeoJSON I/O, S-52/ENC marine GIS engine, OSM tile provider, and `guikit::geo::MapView` |
| [`signal`](https://github.com/guikit/signal) | `guikit::signal` | DSP Waveform + STFT Spectrogram (`SignalView`), Polar Sonar/Radar (`SonarView`), and RTSA Waterfall (`WaterfallView`) |
| [`cockpit`](https://github.com/guikit/cockpit) | `guikit::cockpit` | Avionics Primary Flight Display (`AttitudeView`) and Automotive Instrument Cluster (`CarClusterView`) |
| [`diagram`](https://github.com/guikit/diagram) | `guikit::diagram` | Interactive BPMN 2.0 Modeler & Token Simulator (`BpmnEditor`) |
| `demo/` | `guikit_demo` | Unified 38-screen interactive studio & C++20 code generator showcasing all modules |

---

## Quick Start

### 1. Clone with All Submodules
```bash
git clone --recursive https://github.com/guikit/bundle.git
cd bundle
```

### 2. Desktop Build, Test & Run
**Linux:**
```bash
# Install X11 / Wayland / OpenGL development headers (Ubuntu / Debian / Fedora / Arch)
./scripts/linux/install-deps.sh

# Configure, build & run all ecosystem tests
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure

# Native showcase (100% GuiKit + keel::font + OpenGL ES3 — zero SDL3 / ThorVG / FreeType)
./scripts/linux/native-build.sh
./scripts/linux/native-run.sh

# SDL3 + ThorVG + FreeType showcase
./scripts/linux/sdl-build.sh
./scripts/linux/sdl-run.sh
```

**Windows:**
```cmd
:: Native showcase (zero SDL3 / ThorVG / FreeType)
scripts\windows\native-build.cmd
scripts\windows\native-run.cmd

:: SDL3 + ThorVG + FreeType showcase
scripts\windows\sdl-build.cmd
scripts\windows\sdl-run.cmd
```

### 3. WebAssembly Build, Local Preview & GitHub Pages Deploy
**Linux:**
```bash
# Build WebAssembly bundle (auto-installs local .emsdk if needed)
./scripts/linux/web-build.sh

# Build, start local HTTP server, and open in browser
./scripts/linux/web-run.sh

# Build and publish to GitHub Pages (https://guikit.github.io/demo/)
./scripts/linux/web-publish.sh
```

**Windows:**
```cmd
scripts\windows\web-build.cmd
scripts\windows\web-run.cmd
scripts\windows\web-publish.cmd
```

---

## Using `guikit::all` in CMake

```cmake
add_subdirectory(bundle)
target_link_libraries(my_app PRIVATE guikit::all)
```

## License

BSD Zero Clause License (`0BSD`). See [LICENSE](LICENSE) for details.
