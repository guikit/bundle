# PixelGrid Follow-up: Прямые 1dp рамки в отдельных виджетах вне `StylePainter`

## Контекст

Основной механизм физического pixel-snapping уже реализован и покрыт тестами:
- `ICanvas::GetPixelTransform()` во всех 5 бэкендах (`GlEs3Canvas`, `ThorCanvas`, `SdlCanvas`, `DrawListRecorder`, `NullCanvas`)
- `guikit::PixelGrid` (`guikit/include/guikit/core/pixel_grid.hpp`) с `SnapRectPx`, `SnapRect`, `UnitStepPx`, `StrokePx`, `DrawAlignedLine`
- 100% `DefaultStylePainter` (`style_painter.cpp`) и все 1px-разделители/сетки (`separator.cpp`, `heading.cpp`, `split_button.cpp`, `toolbar.cpp`, `carousel.cpp`, `table.cpp`, `tree_view.cpp`)

Однако в нескольких виджетах вне `StylePainter` остались прямые вызовы `canvas.DrawRoundRect(r, radius, fill, stroke, 1.0f)` без привязки к `PixelGrid`. На дробных масштабах DPI (`125%`, `150%`, `175%`) или при дробном скролле такие одиночные 1dp-рамки могут иметь слегка разную толщину/яркость противоположных сторон.

---

## Кандидаты на перевод на `PixelGrid`

Проверить визуально на `150%` DPI (Windows 98 / Modern / Brutalist) и при необходимости перевести на `PixelGrid::From(canvas)` (`pg.SnapRect(r)` + `pg.UnitStepDp()`):

- [ ] **Редактор кода и поля ввода**
  - `guikit/src/guikit/fields/text_area_render.cpp` (подсветка совпадений, рамки диагностик/подсказок, мини-элементы)
  - `guikit/src/guikit/fields/color_picker.cpp` (рамки палитры/свотчей)
  - `guikit/src/guikit/fields/date_picker.cpp` (рамки ячеек календаря / индикатора сегодня)
  - `guikit/src/guikit/fields/select_multi.cpp` и `guikit/src/guikit/fields/tag_input.cpp` (рамки чипов/тегов)
- [ ] **Панели и вьюверы**
  - `guikit/src/guikit/surfaces/dock_area.cpp` (рамки предпросмотра drop-зон и плавающих панелей)
  - `guikit/src/guikit/views/markdown.cpp` (рамки блоков кода, таблиц и цитат)
  - `guikit/src/guikit/views/image_view.cpp` и `guikit/src/guikit/views/scene.cpp` (рамки контролов/индикаторов; не трогать трансформируемый контент сцены с плавным зумом!)
- [ ] **Индикаторы**
  - `guikit/src/guikit/display/histogram.cpp`, `level_meter.cpp`, `image.cpp`, `graphics/colormap.cpp`

---

## Ограничения и критерии приёмки

- **Не снэппить анимированные или плавно зумируемые объекты** (`Scene` world-space, бегунки `Slider`/`Switch`, кривые/графики) — снэппить только статичные 1dp UI-рамки.
- Сохранить **0 warnings, 0 errors** в Debug, Release и Native Release, и **21/21 CTest** (включая `guikit_style_architecture_guard` и `*_header_purity_scan`).
