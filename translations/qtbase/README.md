# Qt 标准界面的简体中文后备翻译

`qtbase_zh_CN.ts` 是 Qt 官方 `qttranslations` 仓库 **5.15 分支**中的原始文件，
用于系统没有安装 `qtbase_zh_CN.qm` 时翻译 Qt 标准按钮、文件选择窗口等界面。
文件未经本项目修改，不属于 Gqrx 自有界面的翻译目录。

## 来源与归属

- 上游项目：[Qt Translations](https://code.qt.io/cgit/qt/qttranslations.git/)。
- 固定源码提交：`1462128189bbb41b91d369244b21d0dbc818a291`。
- [该提交中的原始 TS 文件](https://github.com/qt/qttranslations/blob/1462128189bbb41b91d369244b21d0dbc818a291/translations/qtbase_zh_CN.ts)。
- 该文件 SHA-256：`21c2d863020a5fdd86ddf6c30d3875b37795efb7002b47d623e0e4b8dbea3ce8`。
- 最近一次上游更新为 Tyson Tan 的 “Update simplified chinese translation”，
  于 2021 年 6 月 28 日合入；此前相关提交的作者包括 Wenjia Wang 和 Li Xinwei。
  全部翻译由 Qt 社区贡献，完整归属以[上游文件历史](https://github.com/qt/qttranslations/commits/5.15/translations/qtbase_zh_CN.ts)为准。

同一提交的上游许可原文完整保存在 `LICENSE.GPL3-EXCEPT` 中：
GNU General Public License version 3，附 The Qt Company GPL Exception 1.0。
`README.upstream` 同样从该提交原样保留，包含上游对社区翻译及贡献方式的说明。
此目录中的上游文件保留其原有许可与归属，不以 Gqrx 的作者身份重新声明版权。

## 构建方式

构建首先查找当前 Qt 安装提供的 `qtbase_zh_CN.qm`，优先使用系统版本。
只有找不到该文件，或 `QTBASE_ZH_CN_QM` 指向不存在的路径时，才用当前 Qt 的
`lrelease` 编译本目录中的 TS 文件，并以相同的 `:/translations/qtbase_zh_CN.qm`
资源名内嵌。因此构建和运行均不需要下载翻译文件。

后备目录沿用 Qt 5.15 的上下文，包含 Qt 5.12 和 Qt 6 也使用的
`QPlatformTheme`、`QDialogButtonBox`、`QFileDialog` 等标准界面翻译。
较新 Qt 的系统目录可能包含更多新字符串，所以仍然优先使用系统翻译。

可以用独立构建目录验证后备路径，例如：

```sh
cmake -S . -B build-fallback \
  -DQTBASE_ZH_CN_QM:FILEPATH=/nonexistent/qtbase_zh_CN.qm
cmake --build build-fallback
ctest --test-dir build-fallback --output-on-failure
```

应用自有消息的完整性检查仍只读取 `translations/gqrx_zh_CN.ts`；
`update_translations` 仅扫描 `src`，不会更新或合并本目录的上游文件。
