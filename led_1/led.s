
//全局标号
.global _start
//_start函数，程序从这开始完成时钟使能，GPIO初始化，及输出高低电平
_start:

    //使能所有的时钟
    ldr r0, = 0x020c4068//寄存器CCGR0
    ldr r1, = 0xffffffff//所有bit都打开
    str r1, [r0]

    ldr r0, = 0x020c406c//寄存器CCGR1  
    str r1, [r0]

    ldr r0, = 0x020c4070//寄存器CCGR2
    str r1, [r0]

    ldr r0, = 0x020c4074//寄存器CCGR3
    str r1, [r0]

    ldr r0, = 0x020c4078//寄存器CCGR4
    str r1, [r0]

    ldr r0, = 0x020c407c//寄存器CCGR5
    str r1, [r0]

    ldr r0, = 0x020c4080//寄存器CCGR6  
    str r1, [r0]

    //配置GPIO1_IO03这个脚复用为GPIO
    ldr r0, = 0x020e0068//SW_MUX_GPIO1_IO03_BASE寄存器
    ldr r1, = 0x5//写入5，复用为GPIO
    str r1, [r0]

    /*
        bit [16] : HYS关闭
        bit [15:14] : 00 默认下拉
        bit [13] : 0 keeper功能
        bit [12] : 1 pull/keeper使能
        bit [11] : 0 关闭开路输出
        bit [7:6] : 10 速度100MHz
        bit [5:3] : 110 R0/6的驱动能力
        bit [0] : 0 低转换率
     */
    //配置GPIO的电气属性，压摆率，速度，驱动能力等
    ldr r0, = 0x020e02f4//SW_PAD_GPIO1_IO03_BASE寄存器
    ldr r1, = 0x10b0//设为这个值
    str r1, [r0]

    //配置GPIO为输出功能
    ldr r0, = 0x0209c004//GPIO1_GDIR寄存器，设置方向
    ldr r1, = 0x0000008//led脚为输出
    str r1, [r0]

    //设置为低电平，点亮led
    ldr r0, = 0x0209c000//GPIO1_DR寄存器，设置数据
    //ldr r1, =0x0//led脚为低
    ldr r1, = 0xffffffff//led脚为高
    str r1, [r0]
  
    //死循环
loop:
    b loop

    

