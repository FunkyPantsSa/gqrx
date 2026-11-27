# Gqrx 简体中文术语与维护约定

本表统一界面和帮助文档中的译法。标识符、设备名称、协议命令、文件扩展名、物理单位和国际通用缩写保持原样，中文说明用于帮助理解。

| 英文／缩写 | 中文 | 使用说明 |
| --- | --- | --- |
| SDR | 软件定义无线电 | 接收机功能由软件实现。 |
| RF | 射频 | 例如射频增益、射频输入。 |
| IF | 中频 | 与射频和音频增益区分。 |
| AF / audio | 音频 | AF gain 译为“音频增益”。 |
| I/Q | 同相／正交（I/Q） | 在紧凑界面中保留 I/Q，表示两路正交信号。 |
| raw I/Q | 原始 I/Q | 表示未进行音频解调的 I/Q 信号。 |
| demodulation / demodulator | 解调／解调器 | 关闭解调不等于停止 DSP。 |
| AM | 调幅（AM） | 保留模式缩写。 |
| synchronous AM / AM sync | 同步调幅 | 与普通包络检波的 AM 模式区分。 |
| FM | 调频（FM） | 不译为“频率调制器”。 |
| NFM | 窄带调频（NFM） | 常用于窄带语音。 |
| WFM | 宽带调频（WFM） | 有单声道、立体声和 OIRT 制式选项。 |
| SSB | 单边带（SSB） | LSB 与 USB 是其不同边带模式。 |
| LSB / USB | 下边带／上边带 | 接收模式 USB 不译为“通用串行总线”。 |
| CW | 等幅电报（CW） | CW-L / CW-U 中 L/U 表示采用下／上边带接收。 |
| OIRT | OIRT 制式 | 保留制式名称，不擅自改写为其他立体声制式。 |
| gain | 增益 | 与 attenuation（衰减）区分，单位通常为 dB。 |
| AGC | 自动增益控制（AGC） | AGC hang 译为“保持”，decay 按上下文译为“衰减时间”。 |
| squelch / SQL | 静噪 | squelch threshold 译为“静噪门限”。 |
| noise blanker / NB | 噪声消隐器 | 针对脉冲噪声；不与一般“降噪”混用。 |
| DC removal / DC cancellation | 直流消除 | 消除直流分量；不译为直流电源开关。 |
| I/Q balance | I/Q 平衡校正 | 校正 I/Q 幅度和相位不平衡。 |
| I/Q swap | I/Q 互换 | 交换 I 与 Q。 |
| frequency offset | 频率偏移 | 与 FM deviation（调频频偏）区分。 |
| frequency correction | 频率校正 | ppm 值保持原单位，不换算为 Hz。 |
| FM deviation | 调频频偏 | 指调频信号相对载频的最大偏移。 |
| LO | 本振 | local oscillator，不是滤波器低端。 |
| LNB LO | LNB 本振 | LNB 指低噪声降频器，本振频率用于接收频率换算。 |
| filter low cut / high cut | 滤波器低端／高端截止频率 | 参数 LO/HI 要依据滤波器上下文解释。 |
| filter width / bandwidth | 滤波器带宽／带宽 | passband 译为“通带”，以 Hz 表示的数值通常指通带带宽。 |
| filter shape | 滤波器形状 | soft / normal / sharp 译为“平缓／标准／陡峭”。 |
| de-emphasis | 去加重 | 用时间常数表示，不能译成“降噪”。 |
| pre-emphasis | 预加重 | 与去加重对应。 |
| sample rate | 采样率 | sps 表示每秒采样数，保留 ksps / Msps。 |
| decimation / decimator | 抽取／抽取器 | 降低采样率，不笼统译为数据压缩。 |
| FFT | 快速傅里叶变换（FFT） | FFT size 指点数，不是以字节计的文件大小。 |
| FFT bin | FFT 频率单元 | 也可在说明中称“频点”，避免译为容器。 |
| window function | 窗函数 | 不译为界面窗口；Hann、Hamming 等采用约定译名，配置标识保持原样。 |
| spectral leakage | 频谱泄漏 | 与信号泄漏、数据泄漏区分。 |
| panadapter | 频谱显示 | 指可观察接收频段内信号的频谱视图。 |
| waterfall | 瀑布图 | 以颜色随时间显示频谱强度。 |
| peak hold / min hold | 峰值保持／最小值保持 | 保留历史最大／最小频谱值。 |
| averaging | 平均 | FFT averaging 译为“FFT 平均”，不译为平衡。 |
| resolution bandwidth / RBW | 分辨率带宽（RBW） | 保留单位中的 /RBW。 |
| dBFS | 相对满量程的分贝值 | 数字信号的参考电平，不是 dBm。 |
| dBm | 相对 1 mW 的分贝值 | 与 dBV、dBFS 分别保留，不能互换。 |
| dBV | 相对 1 V 的分贝值 | /√Hz 为幅度谱密度归一化单位。 |
| S-meter | S 表（信号强度表） | 不翻译成速率表。 |
| DX cluster / spot | DX 集群／电台通报 | DX 表示远距离通信；spot 指通报的电台信息。 |
| RDS | 广播数据系统（RDS） | 不翻译广播电台实际发送的节目名称和广播文本。 |
| PI / PS / RadioText | 节目标识码／节目服务名称／广播文本 | 协议键 RDS_PI、RDS_PS_NAME、RDS_RADIOTEXT 原样保留。 |
| VFO | 可变频率振荡器（VFO） | 协议中的 VFO 原样保留。 |
| split | 异频收发 | 远程协议兼容选项，不是频谱分割。 |
| AOS / LOS | 信号捕获／信号丢失 | 此处是触发录音启停的事件。 |

## 翻译维护

- 英文源字符串保持稳定，中文译文通过 Qt 翻译目录提供。
- 保留 `%1`、`%2`、`%n` 等占位符；保留富文本标签及链接目标。
- 保留快捷键组合及助记键语义。紧凑控件优先使用简明中文，完整说明放入工具提示。
- 数值、正负号、小数、单位及信号处理配置不随语言切换改变。
- 不改写设备字符串、硬件增益级和调制模式的内部标识符、远程控制命令，以及用户自定义的书签和标签；已知增益级的显示名称可附加中文解释。
- 帮助中的历史发布日期和版本号以英文原始记录为准，不用当前日期替换。
