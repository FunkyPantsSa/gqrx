# 翻译检查

完成常规 CMake 构建后，可运行完整检查：

```sh
ctest --test-dir build --output-on-failure
```

原生 Qt 测试会反复切换中英文，检查接收模式、滤波器、静噪、FFT 点数和帧率、
录制格式、未保存的输入、远程控制设置、视图菜单、默认与自定义频段标签、Qt 标准按钮，
以及接收参数信号和配置文件不被语言切换改变。测试使用临时目录与离屏渲染，无需 SDR 硬件。
它也检查 `0 fps` 保持冻结，以及满采样带宽时频谱平移范围有效。
同一组界面测试会分别使用平台默认样式与 Fusion 样式运行，
确保 Linux 常见的 Fusion 布局路径也在 macOS、Windows 构建中持续覆盖。

需要检查实际控件排版时，可保存测试截图：

```sh
QT_QPA_PLATFORM=offscreen GQRX_TEST_SCREENSHOT_DIR=/tmp/gqrx-ui build/tests/language_switch_test
```

翻译维护时用 `cmake --build build --target update_translations` 更新源字符串，
再用 Qt Linguist 编辑 `translations/gqrx_zh_CN.ts`。常规构建不会改写源目录。

## 无构建环境时检查翻译

在项目根目录运行：

```sh
python3 tests/check_translations.py
```

检查脚本只使用 Python 3 标准库，无需 Qt、GNU Radio、SDR 硬件或额外 Python 包。
它会检查简体中文 TS 目录中所有有效消息是否已完成翻译、译文是否为空、
Qt 占位符（包括 `%1`、`%L1`、`%n`）是否完整，以及富文本的标签结构和链接目标是否保留。
它也会扫描 `src` 下的所有 `.ui` 文件，确认每个可翻译的非空字符串都已覆盖。
检测到问题时输出文件、上下文和原文定位，并以非零状态退出。

若安装了 Qt Linguist 工具，可再使用新提取的 TS 文件验证 C++ 字符串覆盖率：

```sh
lupdate src -no-obsolete -ts /tmp/gqrx-source.ts
python3 tests/check_translations.py --reference /tmp/gqrx-source.ts
```

部分系统的 Qt 工具名为 `lupdate-qt5`、`lupdate6` 或位于 Qt 安装目录的 `bin` 中。
`--reference` 仅用来检查源字符串，不要求参考文件已有译文；不会修改项目目录。

可通过 `--catalog` 和 `--source-root` 指定其他 TS 文件和源码目录。
脚本可发现遗漏和格式损坏，界面裁切、布局、实时切换后的状态保持及专业术语仍需人工或 Qt 运行检查。
