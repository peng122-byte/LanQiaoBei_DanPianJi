# 一、按键

![image-20250913173805207](https://cdn.jsdelivr.net/gh/peng122-byte/my-images@main/img/2025/09/14/image-20250913173805207.png)

```c
unsigned char Key_Read()
{
	unsigned char temp = 0;
	P44 = 0;P42 = 1;P35 = 1;P34 = 1;
	if(P33 == 0) temp = 4;
	if(P32 == 0) temp = 5;
	if(P31 == 0) temp = 6;
	if(P30 == 0) temp = 7;
	P44 = 1;P42 = 0;P35 = 1;P34 = 1;
	if(P33 == 0) temp = 8;
	if(P32 == 0) temp = 9;
	if(P31 == 0) temp = 10;
	if(P30 == 0) temp = 11;
	P44 = 1;P42 = 1;P35 = 0;P34 = 1;
	if(P33 == 0) temp = 12;
	if(P32 == 0) temp = 13;
	if(P31 == 0) temp = 14;
	if(P30 == 0) temp = 15;
	P44 = 1;P42 = 1;P35 = 1;P34 = 0;
	if(P33 == 0) temp = 16;
	if(P32 == 0) temp = 17;
	if(P31 == 0) temp = 18;
	if(P30 == 0) temp = 19;
	return temp;
}
```

# 二、数码管

![image-20250913174158633](https://cdn.jsdelivr.net/gh/peng122-byte/my-images@main/img/image-20250913174158633.png)

![image-20250913174445734](https://cdn.jsdelivr.net/gh/peng122-byte/my-images@main/img/image-20250913174445734.png)

![image-20250914113721803](https://cdn.jsdelivr.net/gh/peng122-byte/my-images@main/img/image-20250914113721803.png)

```c
unsigned char seg_dula[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf};
// 段码表：0~9, 灭, '-'（共阳数码管，低电平点亮）

unsigned char seg_wela[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
// 位码表：第0~7位数码管选通（低电平选通）

void Seg_Disp(unsigned char wela, dula, point)
{
    P0 = 0xff;                     // ① 先关闭所有段（安全清屏）
    P2 = P2 & 0x1f | 0xe0;         // ② 选通“段锁存器”（假设 0xE0 = 段选控制位）
    P2 &= 0x1f;

    P0 = seg_wela[wela];           // ③ 输出位选信号
    P2 = P2 & 0x1f | 0xc0;         // ④ 选通“位锁存器”（假设 0xC0 = 位选控制位）
    P2 &= 0x1f;

    P0 = seg_dula[dula];           // ⑤ 输出段码
    if(point)                      // ⑥ 如果需要显示小数点
        P0 &= 0x7f;                //     清除 bit7（dp 段）
    P2 = P2 & 0x1f | 0xe0;         // ⑦ 重新选通“段锁存器”，锁存段码
    P2 &= 0x1f;	
}
```

