# API 参考手册

## 📚 目录

- [Init 模块](#init-模块)
- [Key 模块](#key-模块)
- [Led 模块](#led-模块)
- [Seg 模块](#seg-模块)
- [IIC 模块](#iic-模块)

---

## Init 模块

### System_Init()

**功能：** 系统初始化函数

**原型：**
```c
void System_Init(void);
```

**参数：** 无

**返回值：** 无

**描述：**
初始化 LED 控制器和蜂鸣器/继电器控制器，关闭所有外设。

**调用示例：**
```c
void main(void) {
    System_Init();  // 第一行调用
    // 其他初始化...
}
```

**实现细节：**
1. 设置 P0 = 0xFF，关闭所有 LED
2. 通过 P2 = 0x80 选中 LED 控制器并锁存
3. 设置 P0 = 0x00，关闭蜂鸣器和继电器
4. 通过 P2 = 0xA0 选中蜂鸣器/继电器控制器并锁存

**位置：** `Driver/Init.c:3-12`

---

## Key 模块

### Key_Read()

**功能：** 矩阵键盘扫描函数

**原型：**
```c
unsigned char Key_Read(void);
```

**参数：** 无

**返回值：**
- `0`：无按键按下
- `4-19`：按键编号（对应 S4-S19）

**描述：**
扫描 4x4 矩阵键盘，返回当前按下的按键编号。

**键盘映射表：**

| 按键 | 编号 | 硬件位置 |
|------|------|----------|
| S4  | 4  | 第1行第1列 |
| S5  | 5  | 第1行第2列 |
| S6  | 6  | 第1行第3列 |
| S7  | 7  | 第1行第4列 |
| S8  | 8  | 第2行第1列 |
| ... | ... | ... |
| S19 | 19 | 第4行第4列 |

**调用示例：**
```c
unsigned char key;
key = Key_Read();
if(key == 4) {
    // S4 被按下
}
```

**注意事项：**
- 建议在定时器中断中调用（10ms 周期）
- 需配合边沿检测逻辑使用，避免连击
- 同时按下多个按键时，返回值不确定

**实现原理：**
1. 逐行拉低行线（P44, P42, P35, P34）
2. 检测列线电平（P33, P32, P31, P30）
3. 根据行列组合确定按键编号

**位置：** `Driver/Key.c:3-27`

---

## Led 模块

### Led_Disp()

**功能：** 控制单个 LED 的点亮或熄灭

**原型：**
```c
void Led_Disp(unsigned char addr, unsigned char enable);
```

**参数：**
- `addr`：LED 编号（0-7，对应 L1-L8）
- `enable`：控制状态
  - `0`：熄灭
  - `非0`：点亮

**返回值：** 无

**描述：**
通过位操作控制单个 LED，使用静态变量缓存状态，仅在状态改变时刷新硬件。

**调用示例：**
```c
Led_Disp(0, 1);  // 点亮 L1
Led_Disp(0, 0);  // 熄灭 L1

// 批量控制
for(i = 0; i < 8; i++) {
    Led_Disp(i, ucLed[i]);
}
```

**性能优化：**
- 使用静态变量 `temp` 缓存当前状态
- 仅在状态改变时执行硬件写入
- 减少 I/O 操作，提高效率

**硬件特性：**
- LED 低电平点亮（P0 输出取反）
- 通过 P2 = 0x80 选中 LED 控制器

**位置：** `Driver/Led.c:3-18`

---

### Beep()

**功能：** 控制蜂鸣器

**原型：**
```c
void Beep(unsigned char flag);
```

**参数：**
- `flag`：控制状态
  - `0`：关闭
  - `非0`：开启

**返回值：** 无

**调用示例：**
```c
Beep(1);  // 开启蜂鸣器
Beep(0);  // 关闭蜂鸣器

// 短促蜂鸣（需配合延时）
Beep(1);
delay_ms(100);
Beep(0);
```

**硬件映射：**
- 控制位：P0.6（0x40）
- 片选信号：P2 = 0xA0

**位置：** `Driver/Led.c:20-35`

---

### Relay()

**功能：** 控制继电器

**原型：**
```c
void Relay(unsigned char flag);
```

**参数：**
- `flag`：控制状态
  - `0`：断开
  - `非0`：吸合

**返回值：** 无

**调用示例：**
```c
Relay(1);  // 继电器吸合
Relay(0);  // 继电器断开
```

**硬件映射：**
- 控制位：P0.4（0x10）
- 片选信号：P2 = 0xA0

**注意事项：**
- 继电器为感性负载，断开时可能产生反向电动势
- 硬件电路应有续流二极管保护

**位置：** `Driver/Led.c:37-52`

---

## Seg 模块

### Seg_Disp()

**功能：** 数码管动态扫描显示

**原型：**
```c
void Seg_Disp(unsigned char wela, unsigned char dula, unsigned char point);
```

**参数：**
- `wela`：位选（0-7），指定要显示的数码管位置
- `dula`：段码索引（0-12），指定要显示的字符
- `point`：小数点控制
  - `0`：不显示小数点
  - `非0`：显示小数点

**返回值：** 无

**段码索引表：**

| dula | 显示字符 | 说明 |
|------|---------|------|
| 0-9 | 0-9 | 数字 |
| 10 | (熄灭) | 全灭 |
| 11 | U | 字母 U |
| 12 | F | 字母 F |

**调用示例：**
```c
// 在定时器中断中调用（1ms 周期）
void Timer0Server() interrupt 1 {
    if(++Seg_Pos == 8) Seg_Pos = 0;
    Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]);
}

// 主程序中设置显示内容
Seg_Buf[0] = 11;      // 位0显示 'U'
Seg_Buf[5] = 2;       // 位5显示 '2'
Seg_Point[5] = 1;     // 位5显示小数点
Seg_Buf[6] = 0;       // 位6显示 '0'
Seg_Buf[7] = 0;       // 位7显示 '0'
// 最终显示：U  2.00
```

**显示流程：**
1. 消影：关闭所有段（P0 = 0xFF）
2. 位选：选中目标数码管（P2 = 0xC0）
3. 段选：输出段码数据（P2 = 0xE0）
4. 小数点：如需显示，P0 &= 0x7F

**注意事项：**
- 必须在中断中高频调用（建议 1ms）
- 扫描频率过低会导致闪烁
- 段码表为共阴极数码管配置

**位置：** `Driver/Seg.c:6-21`

---

## IIC 模块

### Ad_Read()

**功能：** 读取 PCF8591 指定通道的 AD 值

**原型：**
```c
unsigned char Ad_Read(unsigned char addr);
```

**参数：**
- `addr`：ADC 通道地址
  - `0x40`：AIN0
  - `0x41`：AIN1
  - `0x42`：AIN2
  - `0x43`：AIN3

**返回值：**
- 8 位 AD 值（0-255）

**描述：**
通过 I2C 总线读取 PCF8591 指定通道的模拟电压值。

**调用示例：**
```c
unsigned char ad_val;
ad_val = Ad_Read(0x43);  // 读取 AIN3 通道

// 转换为电压值（单位：0.01V）
unsigned int voltage;
voltage = (ad_val * 100) / 51;  // 0-500 对应 0-5.00V
```

**电压换算公式：**
```c
实际电压(V) = (ad_val / 255) * 5
简化公式(0.01V) = (ad_val * 100) / 51
```

**I2C 时序：**
1. START
2. 发送 0x90（设备地址 + 写）
3. 等待 ACK
4. 发送通道地址（addr）
5. 等待 ACK
6. RE-START
7. 发送 0x91（设备地址 + 读）
8. 等待 ACK
9. 接收数据字节
10. 发送 NACK
11. STOP

**位置：** `Driver/iic.c:100-115`

---

### Da_Write()

**功能：** 向 PCF8591 的 DAC 通道写入数据

**原型：**
```c
void Da_Write(unsigned char dat);
```

**参数：**
- `dat`：8 位 DA 值（0-255）

**返回值：** 无

**描述：**
通过 I2C 总线向 PCF8591 的 DAC 通道输出模拟电压。

**调用示例：**
```c
// 输出 2.00V
unsigned int voltage = 200;  // 单位：0.01V
unsigned char da_val = (voltage * 51) / 100;
Da_Write(da_val);

// 输出 3.30V
Da_Write(168);  // (3.30 / 5.00) * 255 ≈ 168
```

**电压换算公式：**
```c
DA值 = (期望电压(V) / 5) * 255
简化公式 = (电压(0.01V) * 51) / 100
```

**输出电压范围：**
- 最小：0V（dat = 0）
- 最大：5V（dat = 255）
- 步进：约 0.0196V

**I2C 时序：**
1. START
2. 发送 0x90（设备地址 + 写）
3. 等待 ACK
4. 发送 0x41（DAC 通道地址）
5. 等待 ACK
6. 发送数据字节（dat）
7. 等待 ACK
8. STOP

**位置：** `Driver/iic.c:118-128`

---

### IIC_Start()

**功能：** 发送 I2C 启动信号

**原型：**
```c
void IIC_Start(void);
```

**时序：**
```
SCL: ‾‾‾‾‾‾‾‾‾‾\_____
SDA: ‾‾‾‾‾\________
```

**位置：** `Driver/iic.c:18-26`

---

### IIC_Stop()

**功能：** 发送 I2C 停止信号

**原型：**
```c
void IIC_Stop(void);
```

**时序：**
```
SCL: _________/‾‾‾‾‾
SDA: _______/‾‾‾‾‾‾‾
```

**位置：** `Driver/iic.c:29-36`

---

### IIC_SendByte()

**功能：** 发送一个字节数据

**原型：**
```c
void IIC_SendByte(unsigned char byt);
```

**参数：**
- `byt`：待发送的字节

**位置：** `Driver/iic.c:65-81`

---

### IIC_RecByte()

**功能：** 接收一个字节数据

**原型：**
```c
unsigned char IIC_RecByte(void);
```

**返回值：** 接收到的字节

**位置：** `Driver/iic.c:84-97`

---

### IIC_WaitAck()

**功能：** 等待从机应答信号

**原型：**
```c
bit IIC_WaitAck(void);
```

**返回值：**
- `0`：收到 ACK
- `1`：收到 NACK

**位置：** `Driver/iic.c:52-62`

---

### IIC_SendAck()

**功能：** 发送应答信号

**原型：**
```c
void IIC_SendAck(bit ackbit);
```

**参数：**
- `ackbit`：
  - `0`：发送 ACK
  - `1`：发送 NACK

**位置：** `Driver/iic.c:39-49`

---

## 🔧 使用最佳实践

### 1. 初始化顺序
```c
void main(void) {
    System_Init();   // 1. 硬件初始化
    Timer0Init();    // 2. 定时器初始化
    EA = 1;          // 3. 开启总中断
    // 4. 进入主循环
}
```

### 2. 中断服务函数
```c
void Timer0Server() interrupt 1 {
    // 高优先级任务（动态扫描）
    if(++Seg_Pos == 8) Seg_Pos = 0;
    Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]);
    Led_Disp(Seg_Pos, ucLed[Seg_Pos]);

    // 低优先级任务（减速计数）
    if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
}
```

### 3. 主循环结构
```c
while(1) {
    Key_Proc();   // 按键处理
    Seg_Proc();   // 业务逻辑
    Led_Proc();   // LED 控制
}
```

---

## ⚠️ 常见错误

### 错误 1：数码管不显示
```c
// 错误：未在中断中调用扫描函数
void Timer0Server() interrupt 1 {
    // 缺少扫描代码
}

// 正确
void Timer0Server() interrupt 1 {
    Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]);
}
```

### 错误 2：LED 控制失效
```c
// 错误：直接操作硬件
P0 = 0x01;

// 正确：使用 API
Led_Disp(0, 1);
```

### 错误 3：I2C 通信失败
```c
// 错误：未检查应答
IIC_SendByte(0x90);
// 应该检查 ACK

// 正确
IIC_SendByte(0x90);
if(IIC_WaitAck() != 0) {
    // 处理错误
}
```

---

**文档版本：** v1.0
**最后更新：** 2026-02-04
