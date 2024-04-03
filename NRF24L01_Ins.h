#ifndef __nRF24L01_Ins_H
#define __nRF24L01_Ins_H

/**********  SPI指令  ***********/
#define nRF_READ_REG    0x00        //000B BBBB读寄存器+BBBBB寄存器地址
#define nRF_WRITE_REG   0x20        //001B BBBB写寄存器+BBBBB寄存器地址
#define RD_RX_PLOAD     0x61        //0110 0001读RX有效数据,1~32字节,读操作从字节0开始,读取结束后，FIFO寄存器中有效数据被清除.接受模式下用
#define WR_TX_PLOAD     0xA0        //1010 0000写TX有效数据,1~32字节。写操作从字节0开始.发送模式下用
#define FLUSH_TX        0xE1        //1110 0001清除TXFIFO寄存器.发送模式下用
#define FLUSH_RX        0xE2        //1110 0010清除RXFIFO寄存器.接收模式下用
#define REUSE_TX_PL     0xE3
#define NOP             0xFF
/********** 寄存器地址  **********/
#define CONFIG          0x00        //配置寄存器
#define EN_AA           0x01        //自动应答使能寄存器
#define EN_RXADDR       0x02        //RX地址使能寄存器
#define SETUP_AW        0x03        //设置所用通道地址宽度寄存器
#define SETUP_RETR      0x04        //自动重发设置寄存器
#define RF_CH           0x05        //射频频率设置寄存器,频率计算公式：2400+RF_CH (Mhz)
#define RF_SETUP        0x06        //射频配置寄存器
#define STATUS          0x07        //状态寄存器
#define OBSERVE_TX      0x08        //发送检测寄存器
#define CD              0x09        //载波检测
#define RX_ADDR_P0      0x0A        //数据通道0接收地址寄存器
#define RX_ADDR_P1      0x0B        //1 ~ 5接收地址设置寄存器,P1 ~ P5共用32位高位地址，由P1设置，P2 ~P4只可以设置8位地址（低8位）
#define RX_ADDR_P2      0x0C
#define RX_ADDR_P3      0x0D
#define RX_ADDR_P4      0x0E
#define RX_ADDR_P5      0x0F

#define TX_ADDR         0x10        //发送地址设置寄存器 RX_ADDR_P0与此地址相等
#define RX_PW_P0        0x11        //接收通道0有效数据宽度设置寄存器
#define RX_PW_P1        0x12        //1 ~ 5有效数据宽度设置寄存器：RX_PW_P1 ~ 5，对应通道1~通道5的接收有效数据宽度，方法同RX_PW_P0。
#define RX_PW_P2        0x13
#define RX_PW_P3        0x14
#define RX_PW_P4        0x15
#define RX_PW_P5        0x16
#define FIFO_STATUS     0x17        //FIFO状态寄存器  RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                                    //bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;

/******   STATUS      *******/
#define MAX_RT  	    0x10        //
#define TX_OK   	    0x20        //
#define RX_OK   	    0x40        //

#endif
