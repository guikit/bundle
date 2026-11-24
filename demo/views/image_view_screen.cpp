#include "keel/math.h"
#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

constexpr keel::u32 kDemoImgW = 1024;
constexpr keel::u32 kDemoImgH = 768;

[[nodiscard]] inline keel::f32 HashNoise2D(keel::u32 x, keel::u32 y, keel::u32 seed) noexcept {
    keel::u32 n = x * 374761393u + y * 668265263u + seed * 1442695041u;
    n = (n ^ (n >> 13u)) * 1274126177u;
    n = n ^ (n >> 16u);
    return keel::as<keel::f32>(n & 0xFFFFu) / 65535.0f;
}

struct ImageViewDemoState {
    bool initialized{false};
    keel::i32 datasetIndex{0}; // 0 = 16-bit Radiographic/NDT Phantom (1ch), 1 = Multi-Channel Fluorescence/Spectral (3ch RGB)

    keel::Array<keel::u16> mono16Pixels{};
    keel::Array<keel::f32> rgbFloatPixels{};
    keel::u64 generationCounter{1};

    ImageViewParams paramsMono{};
    ImageViewParams paramsRgb{};
    ImageViewOptions viewOpts{
        .height = SizeMode::Stretch,
        .showSidebar = true,
        .sidebarOnRight = false,
        .sidebarWidth = 300.0f,
        .showTopToolbar = true,
        .showStatusBar = true,
        .showPixelGridAtHighZoom = true,
        .enableLineProfile = true,
        .enableCtrlWlDrag = true
    };
    ImageViewResponse lastResp{};

    void EnsureGenerated() {
        if (datasetIndex == 0) {
            if (mono16Pixels.empty()) {
                GenerateMono16Phantom();
            }
            if (!rgbFloatPixels.empty()) {
                rgbFloatPixels = keel::Array<keel::f32>{};
            }
        } else {
            if (rgbFloatPixels.empty()) {
                GenerateMultispectralRgb();
            }
            if (!mono16Pixels.empty()) {
                mono16Pixels = keel::Array<keel::u16>{};
            }
        }
        initialized = true;
    }

    void GenerateMono16Phantom() {
        mono16Pixels.resize(keel::as<keel::usize>(kDemoImgW) * kDemoImgH);
        const keel::f32 cx = kDemoImgW * 0.5f;
        const keel::f32 cy = kDemoImgH * 0.54f;

        for (keel::u32 y = 0; y < kDemoImgH; ++y) {
            const keel::f32 ny = keel::as<keel::f32>(y) / keel::as<keel::f32>(kDemoImgH - 1);
            for (keel::u32 x = 0; x < kDemoImgW; ++x) {
                const keel::f32 nx = keel::as<keel::f32>(x) / keel::as<keel::f32>(kDemoImgW - 1);

                // 1. Smooth background gradient (12% .. 82% of 16-bit range)
                keel::f32 v = 8000.0f + 22000.0f * (0.55f * nx + 0.45f * ny);

                // 2. Top 10-step optical density calibration wedge
                if (y >= 28 && y <= 104 && x >= 64 && x < 960) {
                    const keel::i32 step = keel::as<keel::i32>((x - 64) * 10 / (960 - 64));
                    v = 4500.0f + keel::as<keel::f32>(step) * 5800.0f;
                    // Subtle internal calibration pinholes inside each step
                    const keel::f32 stepCenterX = 64.0f + (keel::as<keel::f32>(step) + 0.5f) * (896.0f / 10.0f);
                    const keel::f32 dHole = keel::hypot(keel::as<keel::f32>(x) - stepCenterX, keel::as<keel::f32>(y) - 66.0f);
                    if (dHole < 10.0f) {
                        v += 1800.0f * (1.0f - dHole / 10.0f);
                    }
                }

                // 3. Central radiographic assembly housing & concentric attenuation rings
                const keel::f32 dx = keel::as<keel::f32>(x) - cx;
                const keel::f32 dy = keel::as<keel::f32>(y) - cy;
                const keel::f32 r = keel::hypot(dx, dy);
                if (r < 265.0f) {
                    v += 14000.0f * keel::cos((r / 265.0f) * 1.570796f);
                    // Annular machined grooves
                    if (r > 195.0f && r < 225.0f) {
                        v -= 6500.0f;
                    } else if (r > 110.0f && r < 135.0f) {
                        v += 7200.0f;
                    }
                    // Radial spokes
                    const keel::f32 ang = keel::atan2(dy, dx);
                    const keel::f32 spoke = keel::cos(ang * 8.0f);
                    if (r > 48.0f && r < 185.0f && spoke > 0.86f) {
                        v += 4200.0f * (spoke - 0.86f) / 0.14f;
                    }
                }

                // 4. High-frequency resolution line-pair bar targets (bottom-left)
                if (x >= 72 && x <= 320 && y >= 540 && y <= 700) {
                    const keel::f32 freq = 0.15f + 0.85f * (keel::as<keel::f32>(x - 72) / 248.0f);
                    const keel::f32 wave = keel::sin(keel::as<keel::f32>(x - 72) * freq * 1.3f);
                    v = 26000.0f + (wave >= 0.0f ? 11500.0f : -11500.0f);
                }

                // 5. Low-contrast subtle inclusions array (bottom-right) for W/L inspection
                if (x >= 690 && x <= 955 && y >= 520 && y <= 715) {
                    v = 44000.0f;
                    for (keel::i32 row = 0; row < 3; ++row) {
                        for (keel::i32 col = 0; col < 4; ++col) {
                            const keel::f32 icx = 730.0f + keel::as<keel::f32>(col) * 62.0f;
                            const keel::f32 icy = 555.0f + keel::as<keel::f32>(row) * 60.0f;
                            const keel::f32 rad = 18.0f - keel::as<keel::f32>(row) * 4.0f;
                            const keel::f32 d = keel::hypot(keel::as<keel::f32>(x) - icx, keel::as<keel::f32>(y) - icy);
                            if (d < rad) {
                                const keel::f32 contrast = (800.0f + keel::as<keel::f32>(col) * 1400.0f);
                                v += contrast * (1.0f - (d * d) / (rad * rad));
                            }
                        }
                    }
                }

                // 6. Sensor quantum noise
                const keel::f32 noise = (HashNoise2D(x, y, 1337u) + HashNoise2D(x, y, 7919u) - 1.0f) * 480.0f;
                const keel::f32 clamped = keel::clamp(keel::round(v + noise), 0.0f, 65535.0f);
                mono16Pixels[keel::as<keel::usize>(y) * kDemoImgW + x] = keel::as<keel::u16>(clamped);
            }
        }
    }

    void GenerateMultispectralRgb() {
        rgbFloatPixels.resize(keel::as<keel::usize>(kDemoImgW) * kDemoImgH * 3u);
        const keel::f32 cx = kDemoImgW * 0.5f;
        const keel::f32 cy = kDemoImgH * 0.52f;

        for (keel::u32 y = 0; y < kDemoImgH; ++y) {
            const keel::f32 ny = keel::as<keel::f32>(y) / keel::as<keel::f32>(kDemoImgH - 1);
            for (keel::u32 x = 0; x < kDemoImgW; ++x) {
                const keel::f32 nx = keel::as<keel::f32>(x) / keel::as<keel::f32>(kDemoImgW - 1);
                const keel::f32 dx = keel::as<keel::f32>(x) - cx;
                const keel::f32 dy = keel::as<keel::f32>(y) - cy;
                const keel::f32 r = keel::hypot(dx, dy);
                const keel::f32 ang = keel::atan2(dy, dx);

                // Base background with slight channel offset (ideal for testing per-channel histogram & Auto Balance)
                keel::f32 red   = 180.0f + 420.0f * nx;
                keel::f32 green = 140.0f + 380.0f * ny;
                keel::f32 blue  = 260.0f + 320.0f * (1.0f - 0.5f * (nx + ny));

                // Red Channel: outer filament ring & vascular radial web
                const keel::f32 ringEnv = keel::exp(-((r - 220.0f) * (r - 220.0f)) / (2.0f * 42.0f * 42.0f));
                const keel::f32 web = 0.5f + 0.5f * keel::sin(ang * 12.0f + r * 0.045f);
                red += 2850.0f * ringEnv * (0.35f + 0.65f * web);

                // Green Channel: cellular organelle clusters & top calibration bars
                for (keel::i32 k = 0; k < 6; ++k) {
                    const keel::f32 a = keel::as<keel::f32>(k) * (keel::kTauF / 6.0f);
                    const keel::f32 ox = cx + keel::cos(a) * 135.0f;
                    const keel::f32 oy = cy + keel::sin(a) * 135.0f;
                    const keel::f32 od = keel::hypot(keel::as<keel::f32>(x) - ox, keel::as<keel::f32>(y) - oy);
                    green += 2700.0f * keel::exp(-(od * od) / (2.0f * 36.0f * 36.0f));
                }

                // Blue Channel: bright central core & outer calibration beacons
                blue += 3300.0f * keel::exp(-(r * r) / (2.0f * 68.0f * 68.0f));

                // Top RGB calibration step wedge
                if (y >= 24 && y <= 84 && x >= 64 && x < 960) {
                    const keel::f32 u = keel::as<keel::f32>(x - 64) / 896.0f;
                    if (y < 44) {
                        red = u * 3800.0f; green = 120.0f; blue = 120.0f;
                    } else if (y < 64) {
                        red = 120.0f; green = u * 3800.0f; blue = 120.0f;
                    } else {
                        red = 120.0f; green = 120.0f; blue = u * 3800.0f;
                    }
                }

                const keel::f32 nR = (HashNoise2D(x, y, 101u) - 0.5f) * 75.0f;
                const keel::f32 nG = (HashNoise2D(x, y, 202u) - 0.5f) * 75.0f;
                const keel::f32 nB = (HashNoise2D(x, y, 303u) - 0.5f) * 75.0f;

                const keel::usize base = (keel::as<keel::usize>(y) * kDemoImgW + x) * 3u;
                rgbFloatPixels[base + 0] = keel::clamp(red + nR,   0.0f, 4095.0f);
                rgbFloatPixels[base + 1] = keel::clamp(green + nG, 0.0f, 4095.0f);
                rgbFloatPixels[base + 2] = keel::clamp(blue + nB,  0.0f, 4095.0f);
            }
        }
    }
};

ImageViewDemoState s_imgDemo{};

} // namespace

void RenderImageViewInspector(guikit::Context& ui, DemoContext& /*ctx*/) {
    s_imgDemo.EnsureGenerated();

    guikit::Heading(ui, {.label = "DATASET & CHANNELS", .level = HeadingLevel::H6, .separator = HeadingSeparator::Bottom});
    static constexpr keel::StaticArray<keel::StringRef, 2> kDatasetNames = {
        "16-bit Radiograph Phantom (1ch)",
        "Multispectral Fluorescence (3ch RGB)"
    };
    if (auto el = guikit::Element(ui, {.label = "Synthetic Dataset"})) {
        if (guikit::Select(ui, s_imgDemo.datasetIndex, kDatasetNames).changed) {
            ui.InvalidateAll();
        }
    }

    ImageViewParams& activeParams = (s_imgDemo.datasetIndex == 0) ? s_imgDemo.paramsMono : s_imgDemo.paramsRgb;

    static constexpr keel::StaticArray<keel::StringRef, 3> kHistScales = {"Sqrt", "Log", "Linear"};
    if (auto el = guikit::Element(ui, {.label = "Histogram Scale"})) {
        if (guikit::Segmented(ui, activeParams.histScaleMode, kHistScales).changed) {
            ui.InvalidateAll();
        }
    }

    guikit::Heading(ui, {.label = "WORKSTATION LAYOUT", .level = HeadingLevel::H6, .separator = HeadingSeparator::Bottom});
    if (auto el = guikit::Element(ui, {.label = "Show Built-in Sidebar"})) {
        if (guikit::Switch(ui, s_imgDemo.viewOpts.showSidebar).changed) ui.InvalidateAll();
    }
    static constexpr keel::StaticArray<keel::StringRef, 2> kSidebarSides = {"Left", "Right"};
    keel::usize sideIdx = s_imgDemo.viewOpts.sidebarOnRight ? 1u : 0u;
    if (auto el = guikit::Element(ui, {
        .label = "Sidebar Side",
        .disabled = !s_imgDemo.viewOpts.showSidebar
    })) {
        if (guikit::Segmented(ui, sideIdx, kSidebarSides, {
            .disabled = !s_imgDemo.viewOpts.showSidebar
        }).changed) {
            s_imgDemo.viewOpts.sidebarOnRight = (sideIdx == 1u);
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {
        .label = "Sidebar Width",
        .disabled = !s_imgDemo.viewOpts.showSidebar
    })) {
        if (guikit::Slider(ui, s_imgDemo.viewOpts.sidebarWidth, 240.0f, 380.0f, {
            .valueFormat = "%.0f px",
            .disabled = !s_imgDemo.viewOpts.showSidebar
        }).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Show Top Quick Toolbar"})) {
        if (guikit::Switch(ui, s_imgDemo.viewOpts.showTopToolbar).changed) ui.InvalidateAll();
    }
    UiSize tbSize = s_imgDemo.viewOpts.toolbarSize.value_or(UiSize::Md);
    if (auto el = guikit::Element(ui, {
        .label = "Toolbar Size",
        .disabled = !s_imgDemo.viewOpts.showTopToolbar
    })) {
        if (guikit::StepSlider(ui, tbSize, {
            .disabled = !s_imgDemo.viewOpts.showTopToolbar
        }).changed) {
            s_imgDemo.viewOpts.toolbarSize = tbSize;
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Show Bottom Status Bar"})) {
        if (guikit::Switch(ui, s_imgDemo.viewOpts.showStatusBar).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "High-Zoom Pixel Grid (>=12x)"})) {
        if (guikit::Switch(ui, s_imgDemo.viewOpts.showPixelGridAtHighZoom).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Enable RMB Line Profile"})) {
        if (guikit::Switch(ui, s_imgDemo.viewOpts.enableLineProfile).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Enable Ctrl+LMB W/L Drag"})) {
        if (guikit::Switch(ui, s_imgDemo.viewOpts.enableCtrlWlDrag).changed) ui.InvalidateAll();
    }
}

void RenderImageViewStage(guikit::Context& ui, DemoContext& /*ctx*/) {
    s_imgDemo.EnsureGenerated();

    if (s_imgDemo.datasetIndex == 0) {
        const ImageViewData data = ImageViewData::FromU16(
            s_imgDemo.mono16Pixels,
            kDemoImgW,
            kDemoImgH,
            1,
            1001ull
        );
        s_imgDemo.lastResp = guikit::ImageView(ui, data, s_imgDemo.paramsMono, s_imgDemo.viewOpts);
    } else {
        const ImageViewData data = ImageViewData::FromFloat(
            s_imgDemo.rgbFloatPixels,
            kDemoImgW,
            kDemoImgH,
            3,
            2002ull
        );
        s_imgDemo.lastResp = guikit::ImageView(ui, data, s_imgDemo.paramsRgb, s_imgDemo.viewOpts);
    }
}

keel::String GenerateImageViewCode(const DemoContext& /*ctx*/) {
    const ImageViewParams& prm = (s_imgDemo.datasetIndex == 0) ? s_imgDemo.paramsMono : s_imgDemo.paramsRgb;
    const auto& ch = prm.channels[keel::as<keel::usize>(prm.activeHistChannel)];

    return keel::format(
        "// Single-Channel (16-bit/float) & Multi-Channel (RGB) Scientific ImageView\n"
        "guikit::ImageViewData img = {};\n\n"
        "params.colormap = guikit::Colormap::{};\n"
        "params.channels[0].window = {:.1f}f;\n"
        "params.channels[0].level  = {:.1f}f;\n"
        "params.channels[0].gamma  = {:.2f}f;\n"
        "params.channels[0].invert = {};\n\n"
        "auto resp = guikit::ImageView(ui, img, params, {{\n"
        "    .showSidebar = {},\n"
        "    .sidebarOnRight = {},\n"
        "    .sidebarWidth = {:.0f}f,\n"
        "    .showTopToolbar = {},\n"
        "    .showStatusBar = {},\n"
        "    .enableLineProfile = {}\n"
        "}});",
        (s_imgDemo.datasetIndex == 0)
            ? "guikit::ImageViewData::FromU16(pixels16, 1024, 768, 1)"
            : "guikit::ImageViewData::FromFloat(pixelsRgb, 1024, 768, 3)",
        GetColormapName(prm.colormap),
        ch.window,
        ch.level,
        ch.gamma,
        ch.invert ? "true" : "false",
        s_imgDemo.viewOpts.showSidebar ? "true" : "false",
        s_imgDemo.viewOpts.sidebarOnRight ? "true" : "false",
        s_imgDemo.viewOpts.sidebarWidth,
        s_imgDemo.viewOpts.showTopToolbar ? "true" : "false",
        s_imgDemo.viewOpts.showStatusBar ? "true" : "false",
        s_imgDemo.viewOpts.enableLineProfile ? "true" : "false"
    );
}

void ReleaseImageViewDemoBuffers() noexcept {
    s_imgDemo.mono16Pixels = {};
    s_imgDemo.rgbFloatPixels = {};
    s_imgDemo.initialized = false;
}

} // namespace guikit::demo
