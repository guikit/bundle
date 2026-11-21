#include "bundle_screens.hpp"
#include "demo_window.hpp"
#if defined(GUIKIT_HAS_BACKEND_SDL3) && GUIKIT_HAS_BACKEND_SDL3
#include "guikit/sdl.hpp"
#endif

namespace guikit::demo {
void RegisterDemoEmbeddedAssets() noexcept;
} // namespace guikit::demo

int main(int /*argc*/, char* /*argv*/[]) {
    guikit::demo::RegisterDemoEmbeddedAssets();
    guikit::demo::DemoWindow demo;

    demo.RegisterExtension({
        .navLabel = "GIS",
        .title = "GIS Map — ENC Nautical & OSM Tile Engine",
        .icon = guikit::Icon::Search(),
        .isGpuScene = false,
        .renderInspector = &guikit::demo::RenderOsmMapInspector,
        .renderStage = &guikit::demo::RenderOsmMapStage,
        .generateCode = &guikit::demo::GenerateOsmMapCode,
        .releaseBuffers = &guikit::demo::ReleaseOsmMapDemoBuffers
    });

    demo.RegisterExtension({
        .navLabel = "Signal View",
        .title = "Signal View — DSP Waveform & Spectrogram",
        .icon = guikit::Icon::Play(),
        .isGpuScene = true,
        .renderInspector = &guikit::demo::RenderSignalInspector,
        .renderStage = &guikit::demo::RenderSignalStage,
        .generateCode = &guikit::demo::GenerateSignalCode,
        .releaseBuffers = &guikit::demo::ReleaseSignalDemoBuffers
    });

    demo.RegisterExtension({
        .navLabel = "Sonar Radar",
        .title = "Sonar Radar — DDS Polar Heatmap & Tracker",
        .icon = guikit::Icon::Search(),
        .isGpuScene = true,
        .renderInspector = &guikit::demo::RenderSonarInspector,
        .renderStage = &guikit::demo::RenderSonarStage,
        .generateCode = &guikit::demo::GenerateSonarCode,
        .releaseBuffers = &guikit::demo::ReleaseSonarDemoBuffers
    });

    demo.RegisterExtension({
        .navLabel = "Waterfall",
        .title = "Waterfall — DOA Bearing-Timegram",
        .icon = guikit::Icon::Play(),
        .isGpuScene = true,
        .renderInspector = &guikit::demo::RenderWaterfallInspector,
        .renderStage = &guikit::demo::RenderWaterfallStage,
        .generateCode = &guikit::demo::GenerateWaterfallCode,
        .releaseBuffers = &guikit::demo::ReleaseWaterfallDemoBuffers
    });

    demo.RegisterExtension({
        .navLabel = "Attitude PFD",
        .title = "Attitude PFD — Primary Flight Display",
        .icon = guikit::Icon::Play(),
        .isGpuScene = false,
        .renderInspector = &guikit::demo::RenderAttitudeInspector,
        .renderStage = &guikit::demo::RenderAttitudeStage,
        .generateCode = &guikit::demo::GenerateAttitudeCode,
        .releaseBuffers = nullptr
    });

    demo.RegisterExtension({
        .navLabel = "Car Cluster",
        .title = "Car Cluster — Digital Cockpit Instrument",
        .icon = guikit::Icon::Play(),
        .isGpuScene = false,
        .renderInspector = &guikit::demo::RenderCarDashboardInspector,
        .renderStage = &guikit::demo::RenderCarDashboardStage,
        .generateCode = &guikit::demo::GenerateCarDashboardCode,
        .releaseBuffers = nullptr
    });

    demo.RegisterExtension({
        .navLabel = "BPMN Modeler",
        .title = "BPMN 2.0 Modeler — Workflow Diagram Editor",
        .icon = guikit::Icon::Edit(),
        .isGpuScene = false,
        .renderInspector = &guikit::demo::RenderBpmnInspector,
        .renderStage = &guikit::demo::RenderBpmnStage,
        .generateCode = &guikit::demo::GenerateBpmnCode,
        .releaseBuffers = nullptr
    });

    return demo.Run();
}
