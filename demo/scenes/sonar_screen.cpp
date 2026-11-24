#include "keel/math.h"
#include "bundle_screens.hpp"
#include "guikit/signal.hpp"

namespace guikit::demo {

namespace {

struct DynamicContact {
    keel::i32 id{0};
    keel::FixedString<64> callsign{};
    SonarTargetClass classification{SonarTargetClass::Diver};
    // Cartesian position in meters relative to sensor (X = East, Y = North)
    keel::f32 x{0.0f};
    keel::f32 y{150.0f};
    keel::f32 vx{0.5f};
    keel::f32 vy{-0.8f};
    keel::f32 depthMeters{6.5f};
    keel::f32 snrDb{19.5f};
    keel::f32 echoStrength{0.92f};
    keel::f32 radialSpreadMeters{7.0f};
    keel::f32 angularSpreadDeg{3.2f};
    keel::Array<PolarPoint> trail{};
};

struct SonarDemoState {
    bool initialized{false};
    bool liveSimulation{true};
    keel::i32 scenarioIndex{0};           // 0 = Harbor Defense DDS, 1 = FLS Shipwreck Inspection, 2 = Offshore Rig 360 Sweep
    keel::i32 displayModeIndex{0};        // 0 = PPI_360, 1 = SectorWedge, 2 = BScan
    keel::i32 scanModeIndex{0};           // 0 = MultibeamContinuous, 1 = RotatingSweep
    Colormap colormap{Colormap::Gold};

    keel::f32 maxRangeMeters{500.0f};
    keel::f32 headingDeg{0.0f};
    keel::f32 sectorApertureDeg{120.0f};
    keel::f32 gainDb{3.0f};
    keel::f32 tvgAmount{0.58f};
    keel::f32 clutterThreshold{0.14f};
    keel::f32 persistence{0.84f};
    keel::f32 sweepSpeedDegPerSec{95.0f};
    keel::f32 sweepAngleDeg{0.0f};
    keel::f32 rangeZoom{1.0f};

    bool showRangeRings{true};
    bool showBearingDial{true};
    bool showTargets{true};
    bool showTargetLabels{true};
    bool showColorbar{true};
    bool showEblVrm{true};
    bool showHudOverlay{true};
    bool guardZoneEnabled{true};

    keel::f32 guardMinRange{90.0f};
    keel::f32 guardMaxRange{340.0f};
    keel::f32 guardStartBrg{315.0f};
    keel::f32 guardEndBrg{55.0f};

    keel::i32 selectedTargetId{1};
    keel::f32 eblBearingDeg{22.0f};
    keel::f32 vrmRangeMeters{210.0f};
    bool requestResetView{false};
    bool requestClearPersistence{false};

    keel::u32 numBeams{180};
    keel::u32 numRangeBins{256};
    keel::Array<keel::f32> staticBathymetry{};
    keel::Array<keel::f32> polarMatrix{};
    keel::Array<DynamicContact> contacts{};
    keel::Array<SonarTarget> targetViews{};

    keel::f64 simTimeSec{0.0};
    keel::f64 lastTrailSampleSec{0.0};
    keel::u64 lastWallNs{0};

    SonarViewResponse lastResp{};
};

SonarDemoState s_sonar{};

[[nodiscard]] inline keel::f32 NormalizeDeg(keel::f32 d) noexcept {
    keel::f32 r = keel::fmod(d, 360.0f);
    if (r < 0.0f) r += 360.0f;
    return r;
}

[[nodiscard]] inline keel::f32 AngleDiffDeg(keel::f32 a, keel::f32 b) noexcept {
    return keel::fmod(a - b + 540.0f, 360.0f) - 180.0f;
}

[[nodiscard]] inline keel::f32 FastHashNoise(keel::u32 x, keel::u32 y, keel::u32 seed) noexcept {
    keel::u32 n = x * 374761393u + y * 668265263u + seed * 1442695041u;
    n = (n ^ (n >> 13u)) * 1274126177u;
    n = n ^ (n >> 16u);
    return keel::as<keel::f32>(n & 0xFFFFu) / 65535.0f;
}

void BuildStaticBathymetry(SonarDemoState& st) {
    const keel::usize total = keel::as<keel::usize>(st.numBeams) * st.numRangeBins;
    st.staticBathymetry.assign(total, 0.0f);

    for (keel::u32 b = 0; b < st.numBeams; ++b) {
        const keel::f32 brg = (keel::as<keel::f32>(b) / keel::as<keel::f32>(st.numBeams)) * 360.0f;
        for (keel::u32 r = 0; r < st.numRangeBins; ++r) {
            const keel::f32 rNorm = keel::as<keel::f32>(r) / keel::as<keel::f32>(st.numRangeBins - 1);
            const keel::f32 rng = rNorm * st.maxRangeMeters;

            // Base seabed reverberation + gentle shoal ridges
            const keel::f32 ridge1 = 0.07f * keel::pow(keel::max(0.0f, keel::sin(rNorm * 14.0f + keel::cos(keel::deg_to_rad(brg) * 3.0f))), 2.0f);
            const keel::f32 noise = 0.06f * FastHashNoise(b, r, 42u);
            keel::f32 val = 0.04f + ridge1 + noise;

            // Harbor Breakwater wall between bearing 300..348 deg at ~260..310 m
            if (brg >= 296.0f && brg <= 350.0f) {
                const keel::f32 wallR = 255.0f + (brg - 296.0f) * 1.15f;
                const keel::f32 distToWall = keel::abs(rng - wallR);
                if (distToWall < 16.0f) {
                    const keel::f32 wallEcho = 0.86f * keel::exp(-(distToWall * distToWall) / (2.0f * 5.2f * 5.2f));
                    val += wallEcho * (0.85f + 0.15f * FastHashNoise(b, r, 99u));
                } else if (rng > wallR + 14.0f) {
                    // Acoustic shadow zone behind the stone breakwater!
                    val *= 0.18f;
                }
            }

            // South-East Pier Pilings & Seafloor Pipeline along 065..105 deg
            if (brg >= 62.0f && brg <= 108.0f) {
                const keel::f32 pierR = 175.0f + (brg - 62.0f) * 1.8f;
                const keel::f32 distToPier = keel::abs(rng - pierR);
                if (distToPier < 10.0f) {
                    const keel::f32 pilingMod = 0.5f + 0.5f * keel::cos(keel::deg_to_rad(brg - 62.0f) * 45.0f);
                    val += 0.72f * pilingMod * keel::exp(-(distToPier * distToPier) / (2.0f * 3.8f * 3.8f));
                }
            }

            // Submerged wreck / reef anomaly at (145m, 012 deg)
            {
                const keel::f32 dBrg = AngleDiffDeg(brg, 12.0f);
                const keel::f32 dRng = rng - 142.0f;
                const keel::f32 d2 = (dRng * dRng) / (11.0f * 11.0f) + (dBrg * dBrg) / (3.8f * 3.8f);
                if (d2 < 9.0f) {
                    val += 0.68f * keel::exp(-0.5f * d2);
                }
            }

            st.staticBathymetry[keel::as<keel::usize>(b) * st.numRangeBins + r] = keel::clamp(val, 0.0f, 1.0f);
        }
    }
}

void ApplyScenarioPreset(SonarDemoState& st, keel::i32 presetIdx) {
    st.scenarioIndex = presetIdx;
    st.contacts.clear();

    if (presetIdx == 0) {
        // 0: Harbor Defense DDS (360 PPI or Sector)
        st.displayModeIndex = 0; // PPI_360
        st.scanModeIndex = 0;    // MultibeamContinuous
        st.colormap = Colormap::Gold;
        st.maxRangeMeters = 500.0f;
        st.headingDeg = 0.0f;
        st.sectorApertureDeg = 120.0f;
        st.gainDb = 3.0f;
        st.tvgAmount = 0.58f;
        st.clutterThreshold = 0.14f;
        st.persistence = 0.84f;
        st.guardZoneEnabled = true;
        st.guardMinRange = 90.0f;
        st.guardMaxRange = 340.0f;
        st.guardStartBrg = 315.0f;
        st.guardEndBrg = 55.0f;
    } else if (presetIdx == 1) {
        // 1: Forward-Looking Multibeam Sector Wedge (DDS / FLS)
        st.displayModeIndex = 1; // SectorWedge
        st.scanModeIndex = 0;    // MultibeamContinuous
        st.colormap = Colormap::Jet;
        st.maxRangeMeters = 400.0f;
        st.headingDeg = 8.0f;
        st.sectorApertureDeg = 120.0f;
        st.gainDb = 4.5f;
        st.tvgAmount = 0.65f;
        st.clutterThreshold = 0.15f;
        st.persistence = 0.86f;
        st.guardZoneEnabled = true;
        st.guardMinRange = 80.0f;
        st.guardMaxRange = 290.0f;
        st.guardStartBrg = 325.0f;
        st.guardEndBrg = 52.0f;
    } else {
        // 2: Offshore Platform 360 Rotating Radar / Sonar Sweep
        st.displayModeIndex = 0; // PPI_360
        st.scanModeIndex = 1;    // RotatingSweep
        st.colormap = Colormap::Green;
        st.maxRangeMeters = 500.0f;
        st.headingDeg = 0.0f;
        st.sectorApertureDeg = 120.0f;
        st.gainDb = 4.0f;
        st.tvgAmount = 0.52f;
        st.clutterThreshold = 0.16f;
        st.persistence = 0.90f;
        st.guardZoneEnabled = true;
        st.guardMinRange = 110.0f;
        st.guardMaxRange = 360.0f;
        st.guardStartBrg = 290.0f;
        st.guardEndBrg = 70.0f;
    }

    // Populate dynamic underwater & surface contacts
    st.contacts.push_back(DynamicContact{
        .id = 1,
        .callsign = "Target-A",
        .classification = SonarTargetClass::Diver,
        .x = 68.0f,
        .y = 215.0f,
        .vx = -0.45f,
        .vy = -1.05f,
        .depthMeters = 6.8f,
        .snrDb = 21.4f,
        .echoStrength = 0.95f,
        .radialSpreadMeters = 6.5f,
        .angularSpreadDeg = 3.0f
    });
    st.contacts.push_back(DynamicContact{
        .id = 2,
        .callsign = "Target-B",
        .classification = SonarTargetClass::Diver,
        .x = -75.0f,
        .y = 195.0f,
        .vx = 0.58f,
        .vy = -0.82f,
        .depthMeters = 8.4f,
        .snrDb = 18.9f,
        .echoStrength = 0.90f,
        .radialSpreadMeters = 6.5f,
        .angularSpreadDeg = 3.0f
    });
    st.contacts.push_back(DynamicContact{
        .id = 3,
        .callsign = "Target-C",
        .classification = SonarTargetClass::UUV,
        .x = 135.0f,
        .y = 140.0f,
        .vx = -1.65f,
        .vy = 1.25f,
        .depthMeters = 14.5f,
        .snrDb = 26.2f,
        .echoStrength = 0.98f,
        .radialSpreadMeters = 8.5f,
        .angularSpreadDeg = 3.6f
    });
    st.contacts.push_back(DynamicContact{
        .id = 4,
        .callsign = "PATROL-07",
        .classification = SonarTargetClass::SurfaceCraft,
        .x = -195.0f,
        .y = -110.0f,
        .vx = 2.40f,
        .vy = 1.90f,
        .depthMeters = 1.2f,
        .snrDb = 31.0f,
        .echoStrength = 1.0f,
        .radialSpreadMeters = 11.0f,
        .angularSpreadDeg = 4.2f
    });
    st.contacts.push_back(DynamicContact{
        .id = 5,
        .callsign = "BIO-SHL4",
        .classification = SonarTargetClass::Biologic,
        .x = 35.0f,
        .y = -180.0f,
        .vx = -0.75f,
        .vy = 0.35f,
        .depthMeters = 11.0f,
        .snrDb = 14.2f,
        .echoStrength = 0.76f,
        .radialSpreadMeters = 15.0f,
        .angularSpreadDeg = 6.5f
    });

    st.selectedTargetId = 1;
    st.requestClearPersistence = true;
    BuildStaticBathymetry(st);
}

void StepSonarSimulation(SonarDemoState& st) {
    const keel::u64 nowNs = keel::monotonic_ns();
    if (!st.initialized) {
        st.initialized = true;
        st.lastWallNs = nowNs;
        ApplyScenarioPreset(st, st.scenarioIndex);
    }

    keel::f64 dt = keel::as<keel::f64>(nowNs - st.lastWallNs) * 1e-9;
    st.lastWallNs = nowNs;
    dt = keel::clamp(dt, 0.0, 0.08);

    if (st.liveSimulation) {
        if (dt < 0.004) dt = 0.016; // deterministic fallback for headless/unit-test frames
        st.simTimeSec += dt;
        st.sweepAngleDeg = NormalizeDeg(st.sweepAngleDeg + keel::as<keel::f32>(dt) * st.sweepSpeedDegPerSec);

        const bool recordTrail = (st.simTimeSec - st.lastTrailSampleSec) >= 0.45;
        if (recordTrail) {
            st.lastTrailSampleSec = st.simTimeSec;
        }

        // Update dynamic contact kinematics (scaled 5x speed so motion is visually engaging in real time)
        const keel::f32 kinematicScale = 5.5f;
        for (auto& c : st.contacts) {
            c.x += c.vx * keel::as<keel::f32>(dt) * kinematicScale;
            c.y += c.vy * keel::as<keel::f32>(dt) * kinematicScale;

            // Gentle sinusoidal maneuvering & boundary turnaround
            const keel::f32 dist = keel::sqrt(c.x * c.x + c.y * c.y);
            if (dist < 45.0f || dist > st.maxRangeMeters * 0.88f) {
                c.vx = -c.vx;
                c.vy = -c.vy;
            }

            if (recordTrail) {
                const keel::f32 rMeters = keel::sqrt(c.x * c.x + c.y * c.y);
                const keel::f32 brgDeg = NormalizeDeg(keel::rad_to_deg(keel::atan2(c.x, c.y)));
                c.trail.push_back(PolarPoint{rMeters, brgDeg});
                if (c.trail.size() > 18) {
                    c.trail.erase(0);
                }
            }
        }
    }

    // Rebuild `targetViews` and synthesize live `polarMatrix`
    st.targetViews.clear();
    st.targetViews.reserve(st.contacts.size());

    const keel::usize totalCells = keel::as<keel::usize>(st.numBeams) * st.numRangeBins;
    if (st.staticBathymetry.size() != totalCells) {
        BuildStaticBathymetry(st);
    }
    st.polarMatrix = st.staticBathymetry.clone();

    // Add time-varying acoustic speckle noise
    const keel::u32 frameSeed = keel::as<keel::u32>(st.simTimeSec * 30.0);
    for (keel::usize i = 0; i < totalCells; ++i) {
        const keel::f32 speckle = (FastHashNoise(keel::as<keel::u32>(i), frameSeed, 7u) - 0.5f) * 0.07f;
        st.polarMatrix[i] = keel::clamp(st.polarMatrix[i] + speckle, 0.0f, 1.0f);
    }

    // Inject moving target acoustic blooms & wakes into `polarMatrix`
    for (const auto& c : st.contacts) {
        const keel::f32 rMeters = keel::sqrt(c.x * c.x + c.y * c.y);
        const keel::f32 brgDeg = NormalizeDeg(keel::rad_to_deg(keel::atan2(c.x, c.y)));
        const keel::f32 speed = keel::sqrt(c.vx * c.vx + c.vy * c.vy);
        const keel::f32 courseDeg = NormalizeDeg(keel::rad_to_deg(keel::atan2(c.vx, c.vy)));

        st.targetViews.push_back(SonarTarget{
            .id = c.id,
            .callsign = c.callsign,
            .classification = c.classification,
            .rangeMeters = rMeters,
            .bearingDeg = brgDeg,
            .speedMetersPerSec = speed,
            .courseDeg = courseDeg,
            .depthMeters = c.depthMeters,
            .snrDb = c.snrDb,
            .trail = c.trail.clone()
        });

        // Paint 2D polar Gaussian signature into `polarMatrix`
        const keel::f32 invSigR2 = 1.0f / keel::max(4.0f, c.radialSpreadMeters * c.radialSpreadMeters);
        const keel::f32 invSigB2 = 1.0f / keel::max(1.0f, c.angularSpreadDeg * c.angularSpreadDeg);
        for (keel::u32 b = 0; b < st.numBeams; ++b) {
            const keel::f32 beamBrg = (keel::as<keel::f32>(b) / keel::as<keel::f32>(st.numBeams)) * 360.0f;
            const keel::f32 dBrg = AngleDiffDeg(beamBrg, brgDeg);
            if (keel::abs(dBrg) > c.angularSpreadDeg * 3.2f) continue;

            keel::f32* row = st.polarMatrix.data() + keel::as<keel::usize>(b) * st.numRangeBins;
            for (keel::u32 r = 0; r < st.numRangeBins; ++r) {
                const keel::f32 binRng = (keel::as<keel::f32>(r) / keel::as<keel::f32>(st.numRangeBins - 1)) * st.maxRangeMeters;
                const keel::f32 dRng = binRng - rMeters;
                if (keel::abs(dRng) > c.radialSpreadMeters * 3.2f) continue;

                const keel::f32 g = keel::exp(-0.5f * (dRng * dRng * invSigR2 + dBrg * dBrg * invSigB2));
                row[r] = keel::min(1.0f, row[r] + c.echoStrength * g);
            }
        }

        // Paint faint wake trail echoes along historical trail points
        for (keel::usize ti = 0; ti < c.trail.size(); ++ti) {
            const keel::f32 wakeWeight = 0.28f * (keel::as<keel::f32>(ti + 1) / keel::as<keel::f32>(c.trail.size() + 1));
            const keel::f32 trR = c.trail[ti].rangeMeters;
            const keel::f32 trB = c.trail[ti].bearingDeg;
            const keel::u32 bCenter = keel::as<keel::u32>(NormalizeDeg(trB) / 360.0f * st.numBeams) % st.numBeams;
            const keel::u32 rCenter = keel::as<keel::u32>(keel::clamp(trR / st.maxRangeMeters, 0.0f, 0.999f) * st.numRangeBins);
            st.polarMatrix[keel::as<keel::usize>(bCenter) * st.numRangeBins + rCenter] =
                keel::min(1.0f, st.polarMatrix[keel::as<keel::usize>(bCenter) * st.numRangeBins + rCenter] + wakeWeight);
        }
    }
}

} // namespace

void RenderSonarInspector(guikit::Context& ui, DemoContext& /*ctx*/) {
    static constexpr keel::StaticArray<keel::StringRef, 3> kDisplayModeItems = {
        "PPI 360°",
        "Sector",
        "B-Scan"
    };
    static constexpr keel::StaticArray<keel::StringRef, 2> kScanModeItems = {
        "Multibeam",
        "Sweep"
    };

    if (auto el = guikit::Element(ui, {
        .label = "Live Simulation",
        .size = guikit::UiSize::Sm
    })) {
        guikit::Switch(ui, s_sonar.liveSimulation, {.size = guikit::UiSize::Sm});
    }

    if (auto el = guikit::Element(ui, {
        .label = "Display Geometry",
        .layout = guikit::ElementLayout::Top,
        .size = guikit::UiSize::Sm
    })) {
        guikit::Segmented(ui, s_sonar.displayModeIndex, kDisplayModeItems, {.size = guikit::UiSize::Sm});
    }

    if (auto el = guikit::Element(ui, {
        .label = "Scan Mode",
        .layout = guikit::ElementLayout::Top,
        .size = guikit::UiSize::Sm
    })) {
        guikit::Segmented(ui, s_sonar.scanModeIndex, kScanModeItems, {.size = guikit::UiSize::Sm});
    }

    keel::i32 cmapIdx = keel::as<keel::i32>(s_sonar.colormap);
    if (auto el = guikit::Element(ui, {
        .label = "Colormap",
        .layout = guikit::ElementLayout::Top,
        .size = guikit::UiSize::Sm
    })) {
        if (guikit::Select(ui, cmapIdx, guikit::GetAllColormapNames(), {.size = guikit::UiSize::Sm}).changed) {
            s_sonar.colormap = keel::from_underlying<guikit::Colormap>(cmapIdx);
        }
    }

    guikit::Heading(ui, {.label = "Processing", .level = guikit::HeadingLevel::H6, .separator = guikit::HeadingSeparator::Bottom});

    if (auto el = guikit::Element(ui, {
        .label = "Gain",
        .layout = guikit::ElementLayout::Top,
        .size = guikit::UiSize::Sm
    })) {
        guikit::Slider(ui, s_sonar.gainDb, -15.0f, 25.0f, {
            .valueFormat = "%+.1f dB",
            .size = guikit::UiSize::Sm
        });
    }

    if (auto el = guikit::Element(ui, {
        .label = "TVG",
        .layout = guikit::ElementLayout::Top,
        .size = guikit::UiSize::Sm
    })) {
        guikit::Slider(ui, s_sonar.tvgAmount, 0.0f, 1.0f, {
            .valueFormat = "%.2f",
            .size = guikit::UiSize::Sm
        });
    }

    if (auto el = guikit::Element(ui, {
        .label = "Clutter Floor",
        .layout = guikit::ElementLayout::Top,
        .size = guikit::UiSize::Sm
    })) {
        guikit::Slider(ui, s_sonar.clutterThreshold, 0.0f, 0.65f, {
            .valueFormat = "%.2f",
            .size = guikit::UiSize::Sm
        });
    }

    if (auto el = guikit::Element(ui, {
        .label = "Persistence",
        .layout = guikit::ElementLayout::Top,
        .size = guikit::UiSize::Sm
    })) {
        guikit::Slider(ui, s_sonar.persistence, 0.10f, 0.96f, {
            .valueFormat = "%.2f",
            .size = guikit::UiSize::Sm
        });
    }

    if (s_sonar.displayModeIndex != 0) {
        if (auto el = guikit::Element(ui, {
            .label = "Sector Aperture",
            .layout = guikit::ElementLayout::Top,
            .size = guikit::UiSize::Sm
        })) {
            guikit::Slider(ui, s_sonar.sectorApertureDeg, 60.0f, 180.0f, {
                .valueFormat = "%.0f deg",
                .size = guikit::UiSize::Sm
            });
        }
    }

    guikit::Heading(ui, {.label = "Overlays", .level = guikit::HeadingLevel::H6, .separator = guikit::HeadingSeparator::Bottom});

    if (auto el = guikit::Element(ui, {.label = "Guard Zone", .size = guikit::UiSize::Sm})) {
        guikit::Switch(ui, s_sonar.guardZoneEnabled, {.size = guikit::UiSize::Sm});
    }
    if (auto el = guikit::Element(ui, {.label = "Targets", .size = guikit::UiSize::Sm})) {
        guikit::Switch(ui, s_sonar.showTargets, {.size = guikit::UiSize::Sm});
    }
    if (auto el = guikit::Element(ui, {.label = "Target Labels", .size = guikit::UiSize::Sm})) {
        guikit::Switch(ui, s_sonar.showTargetLabels, {.size = guikit::UiSize::Sm});
    }
    if (auto el = guikit::Element(ui, {.label = "Range Rings", .size = guikit::UiSize::Sm})) {
        guikit::Switch(ui, s_sonar.showRangeRings, {.size = guikit::UiSize::Sm});
    }
    if (auto el = guikit::Element(ui, {.label = "Bearing Dial", .size = guikit::UiSize::Sm})) {
        guikit::Switch(ui, s_sonar.showBearingDial, {.size = guikit::UiSize::Sm});
    }
    if (auto el = guikit::Element(ui, {.label = "EBL / VRM", .size = guikit::UiSize::Sm})) {
        guikit::Switch(ui, s_sonar.showEblVrm, {.size = guikit::UiSize::Sm});
    }
    if (auto el = guikit::Element(ui, {.label = "Colorbar", .size = guikit::UiSize::Sm})) {
        guikit::Switch(ui, s_sonar.showColorbar, {.size = guikit::UiSize::Sm});
    }

    if (guikit::Button(ui, {
        .text = "Reset Zoom",
        .icon = guikit::Icon::Refresh(),
        .size = guikit::UiSize::Sm,
        .width = guikit::SizeMode::Stretch,
        .color = guikit::StateColor::Secondary()
    })) {
        s_sonar.requestResetView = true;
    }
}

void RenderSonarStage(guikit::Context& ui, DemoContext& /*ctx*/) {
    StepSonarSimulation(s_sonar);
    if (s_sonar.liveSimulation) {
        ui.RequestRepaint(2);
    }

    // Main Autonomous SonarView Widget
    s_sonar.lastResp = guikit::signal::SonarView(ui, s_sonar.polarMatrix, {
        .numBeams = s_sonar.numBeams,
        .numRangeBins = s_sonar.numRangeBins,
        .displayMode = keel::from_underlying<guikit::SonarDisplayMode>(s_sonar.displayModeIndex),
        .scanMode = keel::from_underlying<guikit::SonarScanMode>(s_sonar.scanModeIndex),
        .colormap = s_sonar.colormap,
        .maxRangeMeters = s_sonar.maxRangeMeters,
        .headingDeg = s_sonar.headingDeg,
        .sectorApertureDeg = s_sonar.sectorApertureDeg,
        .gainDb = s_sonar.gainDb,
        .tvgAmount = s_sonar.tvgAmount,
        .clutterThreshold = s_sonar.clutterThreshold,
        .persistence = s_sonar.persistence,
        .sweepAngleDeg = s_sonar.sweepAngleDeg,
        .targets = s_sonar.targetViews,
        .guardZone = {
            .enabled = s_sonar.guardZoneEnabled,
            .minRangeMeters = s_sonar.guardMinRange,
            .maxRangeMeters = s_sonar.guardMaxRange,
            .startBearingDeg = s_sonar.guardStartBrg,
            .endBearingDeg = s_sonar.guardEndBrg
        },
        .showRangeRings = s_sonar.showRangeRings,
        .showBearingDial = s_sonar.showBearingDial,
        .showTargets = s_sonar.showTargets,
        .showTargetLabels = s_sonar.showTargetLabels,
        .showColorbar = s_sonar.showColorbar,
        .showEblVrm = s_sonar.showEblVrm,
        .showHudOverlay = s_sonar.showHudOverlay,
        .selectedTargetId = &s_sonar.selectedTargetId,
        .eblBearingDeg = &s_sonar.eblBearingDeg,
        .vrmRangeMeters = &s_sonar.vrmRangeMeters,
        .viewRangeZoom = &s_sonar.rangeZoom,
        .requestResetView = s_sonar.requestResetView,
        .requestClearPersistence = s_sonar.requestClearPersistence,
        .height = guikit::SizeMode::Stretch
    });

    s_sonar.requestResetView = false;
    s_sonar.requestClearPersistence = false;
}

keel::String GenerateSonarCode(const DemoContext& /*ctx*/) {
    static constexpr keel::StaticArray<const char*, 3> kModeEnums = {
        "SonarDisplayMode::PPI_360",
        "SonarDisplayMode::SectorWedge",
        "SonarDisplayMode::BScan"
    };
    static constexpr keel::StaticArray<const char*, 2> kScanEnums = {
        "SonarScanMode::MultibeamContinuous",
        "SonarScanMode::RotatingSweep"
    };
    const keel::StringRef cmapName = guikit::GetColormapName(s_sonar.colormap);

    return keel::format(
        "// Polar Heatmap / Diver Detection Sonar (DDS) / PPI Radar Widget\n"
        "// polarMatrix: row-major [numBeams x numRangeBins] float backscatter grid\n"
        "auto resp = guikit::signal::SonarView(ui, polarMatrix, {{\n"
        "    .numBeams         = {},\n"
        "    .numRangeBins     = {},\n"
        "    .displayMode      = guikit::{},\n"
        "    .scanMode         = guikit::{},\n"
        "    .colormap         = guikit::Colormap::{},\n"
        "    .maxRangeMeters   = {:.1f}f,\n"
        "    .sectorApertureDeg= {:.1f}f,\n"
        "    .gainDb           = {:.1f}f,\n"
        "    .tvgAmount        = {:.2f}f,\n"
        "    .clutterThreshold = {:.2f}f,\n"
        "    .persistence      = {:.2f}f,\n"
        "    .targets          = trackedTargets,\n"
        "    .guardZone        = {{\n"
        "        .enabled          = {},\n"
        "        .minRangeMeters   = {:.1f}f,\n"
        "        .maxRangeMeters   = {:.1f}f,\n"
        "        .startBearingDeg  = {:.1f}f,\n"
        "        .endBearingDeg    = {:.1f}f\n"
        "    }},\n"
        "    .selectedTargetId = &selectedTargetId,\n"
        "    .eblBearingDeg    = &eblBearingDeg,\n"
        "    .vrmRangeMeters   = &vrmRangeMeters,\n"
        "    .height           = guikit::SizeMode::Stretch\n"
        "}});\n"
        "if (resp.guardZoneAlarm) {{\n"
        "    // Handle perimeter intrusion ({} active diver/UUV contacts in guard sector)\n"
        "}}\n",
        s_sonar.numBeams,
        s_sonar.numRangeBins,
        kModeEnums[keel::as<keel::usize>(keel::clamp(s_sonar.displayModeIndex, 0, 2))],
        kScanEnums[keel::as<keel::usize>(keel::clamp(s_sonar.scanModeIndex, 0, 1))],
        cmapName,
        s_sonar.maxRangeMeters,
        s_sonar.sectorApertureDeg,
        s_sonar.gainDb,
        s_sonar.tvgAmount,
        s_sonar.clutterThreshold,
        s_sonar.persistence,
        s_sonar.guardZoneEnabled ? "true" : "false",
        s_sonar.guardMinRange,
        s_sonar.guardMaxRange,
        s_sonar.guardStartBrg,
        s_sonar.guardEndBrg,
        s_sonar.lastResp.guardZoneIntruderCount
    );
}

void ReleaseSonarDemoBuffers() noexcept {
    s_sonar.staticBathymetry = {};
    s_sonar.polarMatrix = {};
    s_sonar.contacts = {};
    s_sonar.targetViews = {};
    s_sonar.initialized = false;
}

} // namespace guikit::demo
