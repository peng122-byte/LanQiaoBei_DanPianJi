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
P0 = 0xff;//LED全灭
P2 = P2 & 0x1f | 0x80;//P2先选中高三位，改变高三位为1000(开门)
P2 = P2 & 0x1f;//把高三位关上

P0 = 0x00;//关继电器、蜂鸣器
P2 = P2 & 0x1f | 0xa0;//1010（关门）
P2 &= 0x1f;


```

![image-20250913113202957](./Day1_基本模块原理.assets/image-20250913113202957.png)

<p style="text-align: center; font-style: italic; font-size: 14px; margin-top: 5px;">继电器与蜂鸣器的锁存器</p>

ULN的输出对输入取反

## LED底层

```c
 
```

