# 防儿童误锁车内远程报警系统

车内独立监测终端。持续检测车门开闭、车内是否有人、车内温度与空气浓度，一旦出现
"车内有人 + 温度/浓度超阈值"的险情，立即蜂鸣器报警并驱动舵机自动开窗通风，
同时通过 4G 模块把状态上报到阿里云 IoT 平台，实现本地闭环处置与远程告警。

## 硬件组成

| 部件 | 规格 / 接口 | 作用 |
| --- | --- | --- |
| 主控 | STM32F103C8（Cortex-M3，64 KB Flash / 20 KB RAM） | 采样、显示、控制、通信 |
| 4G 模组 | Air724，AT 指令，接 USART1 | 阿里云 IoT MQTT 上报与指令接收 |
| 温度传感器 | DS18B20，单总线 | 车内温度，带 CRC 校验，0.1 ℃ 分辨率 |
| 浓度采集 | ADC1 模拟通道 | 0~100 归一化，代码中作为二氧化碳浓度使用 |
| 显示 | OLED，I²C，中文字库 | 三屏界面：状态 / 温度阈值 / 浓度阈值 |
| 执行机构 | SG90 舵机，TIM3_CH3 PWM | 模拟车窗开合，0° 关窗、90° 开窗 |
| 交互 | 4 键键盘、蜂鸣器、继电器 | 阈值设置、声光报警、外设控制 |
| 车门检测 | GPIO `HL` | 车门开 / 关 |
| 有人检测 | GPIO `IN_F` | 车内有人 / 无人 |

## 功能实现

主循环依次轮询四个功能函数（`Core/Src/main.c`）：

| 函数 | 职责 |
| --- | --- |
| `Key_function()` | 按键扫描，切换显示页、增减阈值、手动开关窗 |
| `Monitor_function()` | 500 ms 读温度与 ADC 浓度；1000 ms 调 `Ali_MQTT_Publish()` 上报 |
| `Display_function()` | OLED 三屏渲染 |
| `Manage_function()` | 读车门/有人信号，超阈值时蜂鸣器报警并开窗 |

- **时基**：TIM1 中断产生 1 ms 节拍，派生 500 ms / 1000 ms 软定时
- **阈值**：温度默认 35 ℃，浓度默认 50 %，均可按键调整
- **上行**：`/sys/{ProductKey}/{DeviceName}/thing/event/property/post`
- **下行**：订阅 `/sys/{ProductKey}/{DeviceName}/thing/service/property/set`

## 目录结构

```
.
├── Core/
│   ├── Inc/            外设与 HAL 配置头文件
│   └── Src/            main.c 及 CubeMX 生成的外设初始化
├── Drivers/
│   ├── CMSIS/          ARM CMSIS 核心头文件 + STM32F1xx 器件头文件
│   └── STM32F1xx_HAL_Driver/   ST 官方 HAL 库
├── HAL/                自写驱动
│   ├── Air724/         4G 模组 + 阿里云 IoT MQTT（含凭据配置模板）
│   ├── DS18B20/        温度传感器
│   ├── OLED/           OLED 显示与字库
│   ├── key/            按键扫描
│   └── delay/          阻塞延时
├── MDK-ARM/            Keil 工程（Project_C8.uvprojx）
├── Project_C8.ioc      STM32CubeMX 工程配置
└── .mxproject
```

## 构建

**环境**：Keil MDK 5 + Keil.STM32F1xx_DFP 器件包（工程使用 2.4.1），目标器件 `STM32F103C8`。

1. 用 Keil 打开 `MDK-ARM/Project_C8.uvprojx`
2. 编译（工程已配置启动文件 `startup_stm32f103xb.s`、时钟 72 MHz）
3. 通过 J-Link 或 ST-Link 下载到板子

## 凭据配置

阿里云 IoT 三元组（ProductKey / DeviceName / DeviceSecret）**不写入源码**。
`HAL/Air724/Air724.h` 只保留占位值，并会尝试包含同目录下的本地配置文件：

```c
#if defined(__has_include)
#  if __has_include("Air724_config_local.h")
#    include "Air724_config_local.h"
#  endif
#endif
```

使用步骤：

1. 复制 `HAL/Air724/Air724_config.example.h` 为 `Air724_config_local.h`（同目录）
2. 填入自己的 `ALIYUN_PRODUCT_KEY` / `ALIYUN_DEVICE_NAME` / `ALIYUN_DEVICE_SECRET` /
   `ALIYUN_TIMESTAMP` / `TELL_PHONE_NUMBER`
3. `Air724_config_local.h` 已在 `.gitignore` 中，不会被提交

没有本地配置文件时工程仍可编译，只是连不上阿里云 IoT；`TELL_PHONE_NUMBER` 为空时
不发短信、不拨号。

## 来源与致谢

- 工程骨架由 **STM32CubeMX** 生成
- `Drivers/STM32F1xx_HAL_Driver` 与 `Drivers/CMSIS` 为 STMicroelectronics / ARM 官方提供
- `HAL/OLED`、`HAL/key`、`HAL/delay` 等驱动参考了正点原子（ALIENTEK）公开例程
- `HAL/Air724` 驱动与业务逻辑为本项目编写

## License

[MIT](LICENSE) —— 第三方组件（ST HAL、CMSIS）遵循其各自许可证。
