# 蓝桥杯单片机 - PCF8591 AD/DA 电压采集与输出系统

## 📋 项目概述

基于 **STC15F2K60S2** 单片机和 **PCF8591** AD/DA 芯片的电压采集与输出系统，实现模拟电压的实时采集、可调输出、数码管显示和 LED 状态指示功能。

### 核心功能

- ✅ 模拟电压采集（0-5.00V，分辨率 0.01V）
- ✅ 可编程电压输出（固定模式/跟随模式）
- ✅ 数码管实时显示（支持显示开关）
- ✅ LED 状态指示（4 路独立控制）
- ✅ 矩阵键盘控制（4x4 矩阵，使用 4 键）

---

## 🔌 硬件配置

### MCU 规格
- **型号：** STC15F2K60S2
- **工作频率：** 12MHz
- **内存：** 60KB Flash + 2KB RAM

### 外设映射

| 外设 | 接口 | 说明 |
|------|------|------|
| **PCF8591** | I2C (P2.0/P2.1) | 8位AD/DA芯片 |
| **数码管** | P0 + P2 | 8位共阴极数码管 |
| **LED** | P0 + P2 | 8位LED指示灯 |
| **按键** | P3 + P4 | 4x4矩阵键盘 |
| **蜂鸣器** | P0.6 (P2=0xA0) | 可选外设 |
| **继电器** | P0.4 (P2=0xA0) | 可选外设 |

### PCF8591 配置
- **I2C 地址：** 0x90 (写) / 0x91 (读)
- **ADC 通道：** AIN3 (0x43)
- **DAC 通道：** AOUT (0x41)
- **分辨率：** 8位 (0-255)
- **参考电压：** 5V

---

## 🏗️ 软件架构

### 目录结构

```
demo7/
├── Driver/                 # 底层驱动层
│   ├── Init.c/h           # 系统初始化
│   ├── Key.c/h            # 矩阵键盘驱动
│   ├── Led.c/h            # LED/蜂鸣器/继电器控制
│   ├── Seg.c/h            # 数码管显示驱动
│   └── iic.c/h            # I2C总线 + PCF8591驱动
├── User/
│   ├── main.c             # 主程序逻辑
│   └── PCF8591.uvproj     # Keil项目文件
└── README.md              # 项目文档
```

### 分层设计

```
┌─────────────────────────────────────┐
│        应用层 (main.c)               │
│  - 业务逻辑                          │
│  - 状态管理                          │
│  - 数据处理                          │
├─────────────────────────────────────┤
│        驱动层 (Driver/)              │
│  - 硬件抽象                          │
│  - 底层I/O操作                       │
│  - 中断服务                          │
├─────────────────────────────────────┤
│        硬件层                         │
│  - STC15F2K60S2                     │
│  - PCF8591                          │
│  - 外设接口                          │
└─────────────────────────────────────┘
```

---

## 📦 模块说明

### 1. Init 模块 - 系统初始化

**文件：** `Driver/Init.c`

**功能：** 初始化 LED 控制器和蜂鸣器/继电器控制器

```c
void System_Init(void);  // 系统初始化函数
```

**实现细节：**
- 关闭所有 LED（P0=0xFF → P2=0x80）
- 关闭蜂鸣器和继电器（P0=0x00 → P2=0xA0）

---

### 2. Key 模块 - 矩阵键盘扫描

**文件：** `Driver/Key.c`

**功能：** 4x4 矩阵键盘扫描（返回键码 4-19）

```c
unsigned char Key_Read(void);  // 返回0表示无按键，4-19表示按键编号
```

**键盘映射：**

| 按键 | 编号 | 功能 |
|------|------|------|
| S4 | 4 | 切换显示模式（电压显示 ↔ 电压输出） |
| S5 | 5 | 切换电压模式（固定 2.00V ↔ 跟随输入） |
| S6 | 6 | LED 显示开关 |
| S7 | 7 | 数码管显示开关 |

**扫描原理：**
- 行线逐行拉低，列线检测
- 采用边沿检测避免连击

---

### 3. Led 模块 - LED/蜂鸣器/继电器控制

**文件：** `Driver/Led.c`

**API 接口：**

```c
void Led_Disp(unsigned char addr, unsigned char enable);  // 控制单个LED
void Beep(unsigned char flag);                            // 蜂鸣器控制
void Relay(unsigned char flag);                           // 继电器控制
```

**技术特点：**
- 使用静态变量缓存状态，减少硬件刷新
- 支持 8 路 LED 独立控制（addr: 0-7）

---

### 4. Seg 模块 - 数码管显示驱动

**文件：** `Driver/Seg.c`

**API 接口：**

```c
void Seg_Disp(unsigned char wela, unsigned char dula, unsigned char point);
// wela: 位选（0-7）
// dula: 段码索引（0-12）
// point: 小数点（0/1）
```

**段码表：**

| 索引 | 显示 | 段码值 |
|------|------|--------|
| 0-9 | 数字 0-9 | 0xC0-0x90 |
| 10 | 熄灭 | 0xFF |
| 11 | 字母 U | 0xC1 |
| 12 | 字母 F | 0x8E |

**显示格式：**
- 位 0：模式标识（U=电压显示，F=电压输出）
- 位 5-7：电压值（X.XX 格式）

---

### 5. IIC 模块 - PCF8591 驱动

**文件：** `Driver/iic.c`

**上层 API：**

```c
unsigned char Ad_Read(unsigned char addr);  // 读取AD值（0-255）
void Da_Write(unsigned char dat);           // 写入DA值（0-255）
```

**底层 I2C 函数：**

```c
void IIC_Start(void);                       // 启动信号
void IIC_Stop(void);                        // 停止信号
void IIC_SendByte(unsigned char byt);       // 发送字节
unsigned char IIC_RecByte(void);            // 接收字节
bit IIC_WaitAck(void);                      // 等待应答
void IIC_SendAck(bit ackbit);               // 发送应答
```

**电压换算公式：**

```c
// AD采集：将8位AD值转换为电压值（单位：0.01V）
dat = (Ad_Read(0x43) * 100) / 51;  // 范围：0-500 (0-5.00V)

// DA输出：将电压值转换为8位DA值
Da_Write(Voltage * 51 / 100);      // Voltage范围：0-500
```

**数学原理：**
- PCF8591 分辨率：5V / 255 = 0.0196V
- 归一化系数：255 / 500 ≈ 51 / 100

---

### 6. Main 模块 - 主程序逻辑

**文件：** `User/main.c`

#### 全局变量

```c
unsigned char Display_Mode;   // 0=电压显示界面，1=电压输出界面
unsigned int Voltage;         // 输出电压值（单位：0.01V）
unsigned int dat;             // AD采集值（单位：0.01V）
bit Led_Display;              // LED显示开关
bit Seg_Display;              // 数码管显示开关
bit Voltage_Mode;             // 0=固定200(2.00V)，1=跟随AD值
```

#### 核心函数

**Key_Proc()** - 按键处理（10ms 周期）
- S4：切换 Display_Mode（0 ↔ 1）
- S5：切换 Voltage_Mode（0 ↔ 1）
- S6：切换 Led_Display（0 ↔ 1）
- S7：切换 Seg_Display（0 ↔ 1）

**Seg_Proc()** - AD/DA 处理与数码管显示（500ms 周期）
```c
1. 读取 AD 值：dat = (Ad_Read(0x43) * 100) / 51
2. 写入 DA 值：Da_Write(Voltage * 51 / 100)
3. 电压模式控制：
   - Voltage_Mode = 0 → Voltage = 200
   - Voltage_Mode = 1 → Voltage = dat
4. 数码管显示更新
```

**Led_Proc()** - LED 指示逻辑
- L1：Display_Mode == 0 时点亮
- L2：Display_Mode == 1 时点亮
- L3：根据电压分段闪烁
  - `< 1.50V`：灭
  - `1.50-2.50V`：亮
  - `2.50-3.50V`：灭
  - `≥ 3.50V`：亮
- L4：Voltage_Mode 状态指示

**Timer0Server()** - 定时器中断（1ms）
- 按键减速计数（10ms）
- 数码管减速计数（500ms）
- 数码管/LED 动态扫描（8 位轮询）

---

## 🚀 使用说明

### 开发环境

- **IDE：** Keil C51 μVision5
- **编译器：** C51 V9.x
- **烧录工具：** STC-ISP
- **调试：** 串口调试助手（可选）

### 编译步骤

1. 打开 Keil 项目：`User/PCF8591.uvproj`
2. 选择目标：Target 1
3. 编译：`F7` 或 `Project → Build Target`
4. 生成 HEX 文件：`User/Objects/PCF8591.hex`

### 烧录步骤

1. 打开 STC-ISP 软件
2. 选择芯片型号：STC15F2K60S2
3. 选择 HEX 文件
4. 设置波特率：115200
5. 点击"下载/编程"，给开发板上电

### 操作流程

#### 初始状态
- 数码管显示：`U  2.00`（电压显示模式，显示 2.00V）
- LED：全灭（Led_Display = 0）

#### 模式切换
1. **按 S6**：开启 LED 显示
   - L1 点亮（Display_Mode = 0）
   - L3 根据电压变化
   - L4 灭（Voltage_Mode = 0）

2. **按 S4**：切换到电压输出模式
   - 数码管显示：`F  2.00`
   - L1 灭，L2 亮

3. **按 S5**：启用电压跟随模式
   - Voltage 跟随 AD 输入变化
   - L4 点亮

4. **按 S7**：关闭数码管显示
   - 数码管全灭

---

## 🔧 开发指南

### 添加新功能

#### 示例：添加电压上下限报警

**步骤 1：定义常量**

```c
#define VOLTAGE_MIN  100   // 1.00V
#define VOLTAGE_MAX  400   // 4.00V
```

**步骤 2：修改 Led_Proc()**

```c
void Led_Proc() {
    // 原有代码...

    // 新增报警逻辑
    if(dat < VOLTAGE_MIN || dat > VOLTAGE_MAX) {
        Beep(1);  // 蜂鸣器报警
    } else {
        Beep(0);
    }
}
```

---

### 调试技巧

#### 1. 使用 LED 调试
```c
// 在关键位置闪烁 LED 指示程序执行
ucLed[7] ^= 1;  // LED8 翻转
```

#### 2. 数码管显示调试值
```c
// 显示 AD 原始值
Seg_Buf[0] = dat / 100;
Seg_Buf[1] = dat / 10 % 10;
Seg_Buf[2] = dat % 10;
```

#### 3. 串口调试（需添加 UART 驱动）
```c
printf("AD Value: %d\r\n", dat);
```

---

### 代码优化建议

#### ⚠️ 待修复问题

1. **变量类型错误** (`main.c:88, 139`)
```c
// 错误
unsigned i;

// 正确
unsigned char i;
```

2. **魔法数字**
```c
// 错误
dat = (Ad_Read(0x43) * 100) / 51;

// 正确
#define ADC_CHANNEL_AIN3  0x43
#define VOLTAGE_SCALE     100
#define ADC_DA_FACTOR     51
dat = (Ad_Read(ADC_CHANNEL_AIN3) * VOLTAGE_SCALE) / ADC_DA_FACTOR;
```

3. **函数职责分离**
```c
// 将 Seg_Proc() 拆分为：
void Adc_Proc(void);   // AD/DA 处理
void Seg_Proc(void);   // 数码管显示
```

---

## 📊 性能参数

| 参数 | 数值 |
|------|------|
| **AD 采样周期** | 500ms |
| **按键扫描周期** | 10ms |
| **数码管刷新频率** | 125Hz (8位轮询 @ 1ms) |
| **电压分辨率** | 0.01V |
| **电压测量范围** | 0-5.00V |
| **电压输出范围** | 0-5.00V |
| **系统响应延迟** | < 500ms |

---

## ❓ 常见问题

### Q1: 数码管不显示
**原因：**
- Seg_Display 被关闭（按了 S7）
- 动态扫描中断未启动

**解决：**
- 按 S7 重新开启
- 检查 `Timer0Init()` 是否调用

---

### Q2: AD 采集值不稳定
**原因：**
- PCF8591 参考电压不稳定
- I2C 时序问题
- 外部干扰

**解决：**
- 加滤波电容（0.1μF）
- 多次采样取平均值
- 缩短 I2C 总线长度

---

### Q3: LED 闪烁异常
**原因：**
- 动态扫描频率过低
- LED 驱动电流不足

**解决：**
- 检查中断是否正常（1ms 周期）
- 检查硬件电路（限流电阻）

---

### Q4: 按键无响应
**原因：**
- 按键减速未工作
- 矩阵扫描逻辑错误

**解决：**
- 检查 `Key_Slow_Down` 计数
- 确认按键硬件连接

---

## 📝 版本历史

### v1.0 (当前版本)
- ✅ 实现 AD/DA 基本功能
- ✅ 数码管显示
- ✅ LED 状态指示
- ✅ 矩阵键盘控制
- ✅ 两种电压输出模式

### 计划功能
- [ ] 串口通信（数据上传）
- [ ] 电压数据存储（EEPROM）
- [ ] 波形发生器（三角波/方波）
- [ ] PID 闭环控制

---

## 📄 许可证

本项目仅用于学习和竞赛目的。

---

## 👥 贡献

欢迎提交 Issue 和 Pull Request！

---

## 📧 联系方式

如有问题请联系：[您的邮箱]

---

**最后更新：** 2026-02-04
