# guikit/bundle

Unified workspace and integration bundle for the **GuiKit** C++20 ecosystem:

- [`guikit`](https://github.com/guikit/guikit) — Core immediate-mode vector GUI library, layout engine, widgets, panels, `Scene`, `ImageView`, and rendering/windowing backends.
- [`geo`](https://github.com/guikit/geo) — Cartography, multi-CRS projections, GeoJSON I/O, S-52/ENC GIS render engine, OSM tile provider, and `guikit::geo::MapView`.
- [`signal`](https://github.com/guikit/signal) — DSP Waveform/Spectrogram (`SignalView`), Polar Sonar/Radar (`SonarView`), and RTSA/BTR Waterfall (`WaterfallView`).
- [`cockpit`](https://github.com/guikit/cockpit) — Avionics Primary Flight Display (`AttitudeView`) and Automotive Instrument Cluster (`CarClusterView`).
- [`diagram`](https://github.com/guikit/diagram) — Interactive BPMN 2.0 Modeler & Token Simulator (`BpmnEditor`).
- `demo/` — Full interactive showcase (`guikit_bundle_demo`) combining all modules.

## Quick Start

```bash
git clone --recursive git@github.com:guikit/bundle.git
cd bundle
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/guikit_bundle_demo
```
