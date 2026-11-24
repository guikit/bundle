#include "../demo_context.hpp"
#include "schematic_primitives.hpp"

namespace guikit::demo {

namespace {
struct SchematicState {
    guikit::SceneBackground bg = guikit::SceneBackground::Solid;
    static constexpr keel::StaticArray<keel::StringRef, 4> bgNames = {
        "Solid", "Checker", "Dots", "Lines"
    };
    bool selectableItems = true;
    bool showControls = true;
    bool showScrollbars = true;
    keel::f32 radius = 8.0f;
    keel::f32 gridStep = 32.0f;
};
SchematicState s_state;
} // namespace

void RenderSchematicInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {.label = "Canvas Backdrop"})) {
        if (guikit::Segmented(ui, s_state.bg, SchematicState::bgNames).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Interactive Item Selection"})) {
        if (guikit::Switch(ui, s_state.selectableItems).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Floating Zoom & Inspector HUD"})) {
        if (guikit::Switch(ui, s_state.showControls).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "2D Scrollbars (H + V)"})) {
        if (guikit::Switch(ui, s_state.showScrollbars).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Viewport Radius"})) {
        if (guikit::Slider(ui, s_state.radius, 0.0f, 20.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Grid Step"})) {
        if (guikit::Slider(ui, s_state.gridStep, 12.0f, 64.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }
}

void RenderSchematicStage(guikit::Context& ui, DemoContext& ctx) {
    const auto schemPal = ctx.previewLightMode
        ? SchematicPalette::VintagePaper()
        : SchematicPalette::DarkCad();
    if (auto scene = guikit::Scene(ui, {
        .worldSize = guikit::Vec2{2200.0f, 920.0f},
        .backgroundStyle = s_state.bg,
        .gridStep = s_state.gridStep,
        .height = guikit::SizeMode::Stretch,
        .selectableItems = s_state.selectableItems,
        .showScrollbars = s_state.showScrollbars,
        .showControls = s_state.showControls,
        .border = {.width = 1.0f, .radius = s_state.radius}
    })) {
        RenderQuad303Schematic(scene, schemPal);
    }
}

keel::String GenerateSchematicCode(const DemoContext& ctx) {
    return keel::format(
        "if (auto scene = ui.Scene({{\n"
        "    .worldSize = guikit::Vec2{{2200.0f, 920.0f}},\n"
        "    .backgroundStyle = guikit::SceneBackground::{},\n"
        "    .gridStep = {:.1f}f,\n"
        "    .height = guikit::SizeMode::Stretch,\n"
        "    .selectableItems = {},\n"
        "    .showScrollbars = {},\n"
        "    .showControls = {},\n"
        "    .border = {{.width = 1.0f, .radius = {:.1f}f}}\n"
        "}})) {{\n"
        "    // Interactive BS 3939 Vector Schematic Primitives via scene.Item(...)\n"
        "    const auto pal = guikit::demo::SchematicPalette::{}();\n"
        "    guikit::demo::RenderQuad303Schematic(scene, pal);\n"
        "}}",
        SchematicState::bgNames[keel::as<keel::usize>(s_state.bg)],
        s_state.gridStep,
        s_state.selectableItems ? "true" : "false",
        s_state.showScrollbars ? "true" : "false",
        s_state.showControls ? "true" : "false",
        s_state.radius,
        ctx.previewLightMode ? "VintagePaper" : "DarkCad"
    );
}

} // namespace guikit::demo
