
/*
汇编代码，为C语言准备运行环境，比如堆栈
*/

//全局标号，汇编代码从_start函数开始
.global _start

_start:
    //进入SVC模式
    mrs r0, cpsr//操作cpsr寄存器
    bic r0, r0, #0x1f//将r0的低5位清零，就是cpsr的M0～M4
    orr r0, r0, #0x13//将r0或上0x13,表示使用SVC模式

    msr cpsr, r0 //将r0数据写入cpsr中，即可改变处理器的模式

/*DDR3地址范围是0X80000000~0XA0000000(512MB) */
    
    ldr sp, = 0x80200000 //设置栈顶指针，所以分配了2MB的栈空间，已经很大了

    b   main            //跳转到main函数


