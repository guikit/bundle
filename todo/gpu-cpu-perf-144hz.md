# Performance Polish: Дожать Native GPU + CPU до уровня 9.5–10/10 (120–144 Гц)

## Текущий baseline (`guikit_demo_native` Release, Theme Studio, `Intel(R) Graphics`, OpenGL 3.3 Core)

- **Оконный `1380×800` @ 60 Гц:** `CPU: ~1.35 ms`, `GPU: ~0.66 ms (4% budget)`
- **Развёрнутый `2560×1494` @ 60 Гц (DPI 1.50):** `CPU: ~1.52–1.79 ms (0.4%)`, `GPU: ~3.38–3.68 ms (20–22% budget)`, `RAM: ~41–67 MB`

Для 60 Гц запас уже отличный (~80% свободного GPU), но на **120–144 Гц** (`6.94 ms` бюджет кадра) текущие `~3.5 ms` GPU занимают ~50% бюджета — при добавлении тяжёлых 2D-сцен (GIS, Sonar, Waterfall) поверх плотного UI запас станет узким.

## Целевые показатели (`2560×1494` HiDPI, iGPU)

- **CPU:** `< 1.0 ms` (снизить с `1.5–1.8 ms`)
- **GPU:** `~1.2–1.5 ms` (`< 22%` бюджета на 144 Гц / `< 10%` на 60 Гц; снизить с `3.4–3.7 ms`)
- **Визуальное качество и архитектура:** 0 деградаций, 0 предупреждений, 21/21 CTest

---

## План доработок

### 1. GPU: Снизить overdraw и число draw calls (`native` + `demo`)

- [ ] **Dual-source blending (`GL_SRC1_COLOR`) для LCD-текста на Desktop OpenGL 3.3** (`native/src/guikit/native/gl_es3_canvas.cpp`, `gl_es3_loader.hpp`)
  - Сейчас каждая смена цвета LCD-текста меняет `glBlendColor` и рвёт батч.
  - На Desktop GL 3.3 Core (с fallback на текущий путь для GLES 3.0 / WebGL 2.0) вывести `index = 0` (premultiplied color) и `index = 1` (per-channel RGB coverage) с `GL_ONE` / `GL_ONE_MINUS_SRC1_COLOR` — весь LCD-текст любых цветов рисуется единым батчем.
- [ ] **Выделенный ultra-lean шейдер для плоских `DrawRect` без `u_ClipCount` / `discard` (опционально) и сокращение overdraw** (`native/src/guikit/native/gl_es3_canvas.cpp`, `guikit/src/guikit/context.cpp`)
  - Проверить замер на `2560×1494`: сколько из `3.5 ms` уходит на `glClear` + вложенные непрозрачные фоны (`Column`/`Box`/`ScrollArea`/`Card` дают 4–6× overdraw на весь экран).
  - Где возможно, избегать лишнего полноэкранного перерисовывания перекрытых непрозрачных фонов или отключать blend (`glDisable(GL_BLEND)`) для полностью непрозрачных `DrawRect` (`fill.a == 255` без rounded-clip), чтобы на iGPU работали fast-path записи в framebuffer без read-modify-write.
- [ ] **Оптимизировать `RenderDotGridBackdrop` в `demo/preview_viewport.hpp`**
  - Сейчас на `2560×1494` рисуется ~2 100 отдельных `DrawRect`-точек, включая область, полностью перекрытую непрозрачным фреймом окна превью.
  - Пропускать область под непрозрачным окном превью или рисовать паттерн процедурным/тайловым способом (1 квад вместо ~2 100).
- [ ] **Опциональный `PlatformWindowDesc::msaaSamples`** (`guikit/include/guikit/backend/i_platform_window.hpp`, `native/src/guikit/native/*_platform_window.cpp`)
  - Сохранить `0` по умолчанию (лучший fill-rate и −56 MB RAM), но дать приложениям с векторными 2D-сценами (GIS/PFD/BPMN на 1× DPI) явную настройку `msaaSamples` (`0 / 2 / 4`).

### 2. CPU: Срезать избыточные ~2 900 вызовов `MeasureText` за кадр (`guikit`)

- [ ] **Аудит и сокращение повторных вызовов `MeasureText` в `Solver` и виджетах** (`guikit/src/guikit/core/solver.cpp`, `fields/input_field.cpp`, `fields/select.cpp`, `actions/button.cpp`, `display/text.cpp`)
  - Сейчас на Theme Studio при 120–220 отображаемых текстовых элементах `MeasureText` вызывается ~2 800–2 976 раз за кадр (~15–23× на строку), что даже с O(1) кешем в `TextEngine` тратит `~0.40–0.45 ms` CPU на хэширование и виртуальные вызовы.
  - Кешировать измеренные размеры текста на время кадра в узле (`Node`) / layout-проходе, чтобы не перемерять ту же строку на `Measure` → `Arrange` → `Render`.
- [ ] **Оптимизировать быстрый путь кеша `MeasureText` в `TextEngine`** (`text/include/guikit/backend/text_engine.hpp`)
  - Оценить после сокращения числа вызовов: при необходимости увеличить размер таблицы до `2048` слотов или добавить 2-way associativity для исключения коллизий на экранах с большим количеством уникальных строк (например, `Table` / `CodeEditor`).

---

## Критерии приёмки

1. Профайлинг Theme Studio в `guikit_demo_native` (Release, `2560×1494`, DPI 1.50, `Intel(R) Graphics`, VSync ON):
   - `CPU <= 1.0 ms`
   - `GPU <= 1.5 ms` (`<= 9%` @ 60 Гц, `<= 22%` @ 144 Гц)
   - Стабильные 60 FPS (и 120/144 FPS на high-refresh мониторе), `0%` CPU в idle (`Reactive Loop`).
2. Проверить визуально все экраны с текстом (LCD/Grayscale), скруглёнными клипами, тенями и 2D-сценами (GIS, Attitude PFD, BPMN, Signal/Sonar/Waterfall).
3. Сборки Debug, Release и `scripts\windows\native-build.cmd Release` — **0 предупреждений, 0 ошибок**, все **21/21** CTest проходят.
