# nrf52840-rt-thread-spim-SPI-DMA--version BSP说明

## 简介

自行实现了 nRF52840 的 SPIM 底层驱动与设备驱动，保留了原厂 SDK 接口与 RT-Thread 的SPI 驱动框架。新增了 drv_spim.c 用于封装设备层接口，并在原厂的 nrfx_spim.c 中扩展了 rtt_nrfx_spim_xfer 和 rtt_spim_xfer 以适配 RT-Thread 的 SPI 驱动模型。已在 board/Kconfig 中添加相应的配置选项。该 SPIM 驱动支持通过 Env 工具的 menuconfig 进行引脚配置与启用，使用方式完全兼容 RT-Thread 官方 SPI 接口规范，支持标准的设备注册、查找及数据传输操作。

## 外设支持

本 BSP 目前对外设的支持情况如下：

| **片上外设** | **支持情况** |          **备注**            |
| :----------- | :----------: | :-------------------------:  |
| GPIO         |     支持     |            GPION             |
| UART         |     支持     |            UART0             |
| PWM          |     支持     |            支持              |
| SPI          |     支持     |            支持              |
| QSPI         |     支持     |   支持开发板上QSPI FLASH      |
| RTC          |     支持     |                              |
| ADC          |     支持     |                              |
| SEGGER_RTT   |     支持     | 打开默认segger_rtt为console   |
| SPIM         |     支持     |                              |



维护人:

-  [supperthomas], 邮箱：<2578307863@qq.com># nrf52840-rt-thread-spim-SPI-DMA--version
