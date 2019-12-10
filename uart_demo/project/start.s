
/*
汇编代码，为C语言准备运行环境，比如堆栈
*/
//全局标号，汇编代码从_start函数开始
.global _start

/*****************************汇编程序从这里开始******************************************/
_start:
    //上电立马执行
    ldr pc, =Rest_Handler       //复位中断
    ldr pc, =Undefined_Handler  //未定义指令中断
    ldr pc, =SVC_Handler        //SVC中断
    ldr pc, =PreAbort_Handler   //预取指令终止
    ldr pc, =DataAbort_Handler  //数据终止
    ldr pc, =NotUsed_Handler  //未使用
    ldr pc, =IRQ_Handler  //外部中断
    ldr pc, =FIQ_Handler  //快速中断

//未初始化的段          
/******************************************************************************************
    一开始放在_start外面谁声明，导致_bss_start在0x87800000了,之后放到了Rest_Handler下方，
    可能被误以为是属于Rest_Handler的代码，然后灯都不闪了！
******************************************************************************************/
.global _bss_start
_bss_start:
    .word __bss_start
_bss_end:
    .word __bss_end

/*  清除bss段   */
    ldr r0, _bss_start
    ldr r1, _bss_end
    mov r2, #0
bss_loop:
    stmia r0!, {r2}
    cmp r0, r1      //比较r0和r1里面的值
    ble bss_loop    //如果r0内值小于r1的，则继续清除bss段

/*******************************复位向量中断****************************************/
Rest_Handler:
    /*
        关闭I-Cache,D-Cache,MMU
        就是要修改SCTRL寄存器，采用读取－修改－写入的方式
        操作cp15协处理器
    */
    cpsid i //关闭全局中断，IRQ

    MRC p15, 0, r0, c1, c0, 0   /*读取SCTRL寄存器数据到r0里面 */

    bic r0, r0, #(1 << 12)//关闭I-Cache
    bic r0, r0, #(1 << 11)//关闭分支预测，就是赌，if语句内加载到Cache
    bic r0, r0, #(1 << 2)//关闭D-Cache
    bic r0, r0, #(1 << 1)//关闭对齐
    bic r0, r0, #(1 << 0)//关闭MMU

    MCR p15, 0, r0, c1, c0, 0   /*将r0的值写入SCTRL寄存器 */

/*
    // 汇编版设置中断向量表偏移
    //设置中断向量偏移
    ldr r0, =0x87800000
    dsb
    isb
    MCR p15, 0, r0, c12, c0, 0
    dsb
    isb
*/
/********************************设置常用模式的sp指针***************************************/
    /******************************设置处理器进入IRQ模式******************************/
    mrs r0, cpsr//读取cpsr寄存器到r0
    bic r0, r0, #0x1f//将r0的低5位清零，就是cpsr的M0～M4
    orr r0, r0, #0x12//将r0或上0x13,写入r0,就是cpsr，表示使用SVC模式
    msr cpsr, r0 //将r0数据写入cpsr中，即可改变处理器的模式
    /*DDR3地址范围是0X80000000~0XA0000000(512MB) */
    //设置IRQ模式下的sp值
    ldr sp, = 0x80600000 //设置栈顶指针，所以分配了2MB的栈空间，已经很大了

    /******************************设置处理器进入SYS模式******************************/
    mrs r0, cpsr//读取cpsr寄存器到r0
    bic r0, r0, #0x1f//将r0的低5位清零，就是cpsr的M0～M4
    orr r0, r0, #0x1f//将r0或上0x13,写入r0,就是cpsr，表示使用SVC模式
    msr cpsr, r0 //将r0数据写入cpsr中，即可改变处理器的模式
    /*DDR3地址范围是0X80000000~0XA0000000(512MB) */
    //设置SYS模式下的sp值
    ldr sp, = 0x80400000 //设置栈顶指针，所以分配了2MB的栈空间，已经很大了

    /******************************设置处理器进入SVC模式******************************/
    mrs r0, cpsr//读取cpsr寄存器到r0
    bic r0, r0, #0x1f//将r0的低5位清零，就是cpsr的M0～M4
    orr r0, r0, #0x13//将r0或上0x13,写入r0,就是cpsr，表示使用SVC模式
    msr cpsr, r0 //将r0数据写入cpsr中，即可改变处理器的模式
    /*DDR3地址范围是0X80000000~0XA0000000(512MB) */
    //设置SVC模式下的sp值
    ldr sp, = 0x80200000 //设置栈顶指针，所以分配了2MB的栈空间，已经很大了

    /******************************打开IRQ，全局中断******************************/
    cpsie i
    /******************************跳转到main函数******************************/
    b   main           
    
/*******************************未定义指令中断*****************************/
Undefined_Handler:
    ldr r0, =Undefined_Handler
    bx r0


/*******************************SVC中断*****************************/
SVC_Handler:
    ldr r0, =SVC_Handler
    bx r0


/*******************************预取指令中断*****************************/
PreAbort_Handler:
    ldr r0, =PreAbort_Handler
    bx r0

/*******************************预取数据中断*****************************/
DataAbort_Handler:
    ldr r0, =DataAbort_Handler
    bx r0

/*******************************未使用中断*****************************/
NotUsed_Handler:
    ldr r0, =NotUsed_Handler
    bx r0

/*******************************IRQ中断*****************************/
IRQ_Handler:
    push {lr}					/* 保存lr地址 */
	push {r0-r3, r12}			/* 保存r0-r3，r12寄存器 */

	mrs r0, spsr				/* 读取spsr寄存器 */
	push {r0}					/* 保存spsr寄存器 */

	mrc p15, 4, r1, c15, c0, 0 /* 从CP15的C0寄存器内的值到R1寄存器中
								* 参考文档ARM Cortex-A(armV7)编程手册V4.0.pdf P49
								* Cortex-A7 Technical ReferenceManua.pdf P68 P138
								*/							
	add r1, r1, #0X2000			/* GIC基地址加0X2000，也就是GIC的CPU接口端基地址 */
	ldr r0, [r1, #0XC]			/* GIC的CPU接口端基地址加0X0C就是GICC_IAR寄存器，
								 * GICC_IAR寄存器保存这当前发生中断的中断号，我们要根据
								 * 这个中断号来绝对调用哪个中断服务函数
								 */
	push {r0, r1}				/* 保存r0,r1 */
	
	cps #0x13					/* 进入SVC模式，允许其他中断再次进去 */
	
	push {lr}					/* 保存SVC模式的lr寄存器 */
	ldr r2, =sys_irqhandle	/* 加载C语言中断处理函数到r2寄存器中*/
	blx r2						/* 运行C语言中断处理函数，带有一个参数，保存在R0寄存器中 */

	pop {lr}					/* 执行完C语言中断服务函数，lr出栈 */
	cps #0x12					/* 进入IRQ模式 */
	pop {r0, r1}				
	str r0, [r1, #0X10]			/* 中断执行完成，写EOIR */

	pop {r0}						
	msr spsr_cxsf, r0			/* 恢复spsr */

	pop {r0-r3, r12}			/* r0-r3,r12出栈 */
	pop {lr}					/* lr出栈 */
	subs pc, lr, #4				/* 将lr-4赋给pc */


/*******************************FIRQ中断*****************************/
FIQ_Handler:
    ldr r0, =FIQ_Handler
    bx  r0


