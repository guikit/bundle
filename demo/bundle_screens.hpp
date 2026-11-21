#pragma once

#include "demo_context.hpp"

namespace guikit::demo {

void RenderOsmMapInspector(guikit::Context& ui, DemoContext& ctx);
void RenderOsmMapStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateOsmMapCode(const DemoContext& ctx);
void ReleaseOsmMapDemoBuffers() noexcept;

void RenderSignalInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSignalStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSignalCode(const DemoContext& ctx);
void ReleaseSignalDemoBuffers() noexcept;

void RenderSonarInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSonarStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSonarCode(const DemoContext& ctx);
void ReleaseSonarDemoBuffers() noexcept;

void RenderWaterfallInspector(guikit::Context& ui, DemoContext& ctx);
void RenderWaterfallStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateWaterfallCode(const DemoContext& ctx);
void ReleaseWaterfallDemoBuffers() noexcept;

void RenderAttitudeInspector(guikit::Context& ui, DemoContext& ctx);
void RenderAttitudeStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateAttitudeCode(const DemoContext& ctx);

void RenderCarDashboardInspector(guikit::Context& ui, DemoContext& ctx);
void RenderCarDashboardStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateCarDashboardCode(const DemoContext& ctx);

void RenderBpmnInspector(guikit::Context& ui, DemoContext& ctx);
void RenderBpmnStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateBpmnCode(const DemoContext& ctx);

} // namespace guikit::demo
