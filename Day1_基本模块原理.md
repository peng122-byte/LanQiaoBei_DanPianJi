# 一、LED模块

<img src="./Day1_基本模块原理.assets/image-20250912184938303.png" alt="image-20250912184938303" style="zoom:50%;" />

<p style="text-align: center; font-style: italic; font-size: 14px; margin-top: 5px;">图1：LED驱动电路</p>

## 锁存器

### RS锁存器

<img src="./Day1_基本模块原理.assets/image-20250912173629234.png" style="zoom:50%;" />

<p style="text-align: center; font-style: italic; font-size: 14px; margin-top: 5px;">图2：RS锁存器</p>

### 加上使能

<img src="./Day1_基本模块原理.assets/image-20250912185912649.png" alt="image-20250912185912649" style="zoom: 67%;" />

<p style="text-align: center; font-style: italic; font-size: 14px; margin-top: 5px;">图3：RS锁存器2</p>

![image-20250913110553025](./Day1_基本模块原理.assets/image-20250913110553025.png)

<p style="text-align: center; font-style: italic; font-size: 14px; margin-top: 5px;">图4：LED锁存器</p>

### 38译码器

![image-20250913110937724](./Day1_基本模块原理.assets/image-20250913110937724.png)

![image-20250913110956229](./Day1_基本模块原理.assets/image-20250913110956229.png)

![image-20250913111745116](./Day1_基本模块原理.assets/image-20250913111745116.png)

## Init

<img src="./Day1_基本模块原理.assets/image-20250913112527591.png" alt="image-20250913112527591" style="zoom:67%;" />

先初始化Led,再初始化继电器、蜂鸣器

```c
void System_Init()
{
	P0 = 0xff;
	P2 = P2 & 0x1f | 0x80;//LED锁存器
	P2 &= 0x1f;
	
	P0 = 0x00;
	P2 = P2 & 0x1f | 0xa0;//继电器、蜂鸣器锁存器
	P2 &= 0x1f;
}

```

![image-20250913113202957](./Day1_基本模块原理.assets/image-20250913113202957.png)

<p style="text-align: center; font-style: italic; font-size: 14px; margin-top: 5px;">继电器与蜂鸣器的锁存器</p>

ULN的输出对输入取反

## LED底层

```c
 void Led_Disp(unsigned char addr, enable) // addr: 灯的地址（0~7），enable: 使能（0 关，1 开）
{
    static unsigned char temp = 0x00;      // 当前所有灯的状态缓存（bit0~bit7 对应灯0~灯7）
    static unsigned char temp_old = 0xff;  // 上一次输出的状态，用于“仅在变化时刷新”，避免频繁操作

    if(enable)
        temp |= (0x01 << addr);            // 使能=1：对应位 置1（开灯）
    else
        temp &= ~(0x01 << addr);           // 使能=0：对应位 清0（关灯）

    if(temp != temp_old)                   // 只有状态变化时才刷新输出，提高效率
    {
        P0 = ~temp;                        // 输出到 P0 口（注意是取反，说明 LED 是共阳接法）
        P2 = P2 & 0x1f | 0x80;             // 打开锁存器使能（假设 0x80 是控制 74HC573 LE 的位）
        P2 &= 0x1f;                        // 关闭锁存器（下降沿锁存数据）
        temp_old = temp;                   // 更新“上一次状态”
    }
}
```

