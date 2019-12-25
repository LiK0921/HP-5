#include "bsp_i2c.h"

void i2c_init(I2C_Type * base)
{
    //close i2c
    base->I2CR &= ~(1 << 7);

    //分频640
    base->IFDR = 0x15;
    //open i2c
    base->I2CR |= (1 << 7);

}

//start信号产生
u8 i2c_master_start(I2C_Type *base, u8 addr, I2C_Direction dir)
{
    //busy?
    if(base->I2SR & (1 << 5))
    {
        return I2C_STATUS_BUSY;
    }
    //set master mode and send mode
    base->I2CR |= (1 << 5) | (1 << 4);

    //start single
    base->I2DR = (u32) ((addr << 1) | (dir == kI2C_Read ? 1 : 0));

    return I2C_STATUS_OK;
}

//stop
u8 i2c_master_stop(I2C_Type *base)
{
    u16 timeout = 0xffff;
    //bit[5:3]清零  
    base->I2CR &= ~((1 << 5) | (1 << 4) | (1 << 3));

    //wait stop
    while ((base->I2SR & (1 << 5)))
    {
        timeout--;
        if (timeout == 0)
        {
            return I2C_STATUS_TIMEOUT;
        }   
    }
    return I2C_STATUS_OK;
}

//repeated start信号产生,用于读取数据的时候
u8 i2c_master_repeated_start(I2C_Type *base, u8 addr, I2C_Direction dir)
{
    //busy && 从机模式下
    if((base->I2SR & (1 << 5)) && ((base->I2CR & (1 << 5)) == 0))
    {
        return I2C_STATUS_BUSY;
    }
    //repeated start and send mode
    base->I2CR |= (1 << 2) | (1 << 4);

    //start single
    base->I2DR = (u32) ((addr << 1) | (dir == kI2C_Read ? 1 : 0));

    return I2C_STATUS_OK;
}



u8 i2c_check_and_clear_err(I2C_Type *base, u32 status)
{
    if (status & (1 << 4))//仲裁丢失错误
    {
        base->I2SR &= ~(1 << 4);
        base->I2SR &= ~(1 << 7);//close i2c
        base->I2SR |= (1 << 7);//open i2c
        return I2C_STATUS_ARBITRATIONLOST;
    }
    else if (status & (1 << 0))//无应答错误
    {
        return I2C_STATUS_NAK;
    }
    else
    {
        /* code */
    }
    return I2C_STATUS_OK;  
}

//i2c 写数据
void i2c_master_write(I2C_Type *base, u8 * buf, u32 size)
{
    //等待传输完成
    while (!(base->I2SR & (1 << 7)))
    {
        /* code */
    }
    
    base->I2SR &= ~(1 << 1);
    base->I2CR |= (1 << 4);

    while (size--)
    {
        base->I2DR = *buf;
        buf++;

        while (!(base->I2SR & (1 << 1))); //等待传输完成,NXP官方SDK包
        base->I2SR &= ~(1 << 1);

        if (i2c_check_and_clear_err(base, base->I2SR))
        {
            break;//退出发送
        }
    }
    base->I2SR &= ~(1 << 1);

    i2c_master_stop(base);

}

//i2c 读数据
void i2c_master_read(I2C_Type *base, u8 * buf, u32 size)
{
    volatile u8 dummy = 0;//假装读取．．．
    dummy++;//防止编译器报警告

    //等待传输完成
    while (!(base->I2SR & (1 << 7)))
    {
        /* code */
    }
    
    base->I2SR &= ~(1 << 1);
    base->I2CR &= ~((1 << 4) | (1 << 3));

    if (size == 1)
    {
        base->I2CR |= (1 << 3);//NO_ACK
    }
    
    dummy = base->I2DR;//假装读取．．．

    while (size--)
    {
        while (!(base->I2SR & (1 << 1))); //等待传输完成,NXP官方SDK包
        base->I2SR &= ~(1 << 1);

        if (size == 0)
        {
            i2c_master_stop(base);
        }
        else if (size == 1)
        {
            base->I2CR |= (1 << 3);//NO_ACK
        }
        *buf = base->I2DR;
        buf++;
    }
}


/*
 * @description	: I2C数据传输，包括读和写
 * @param - base: 要使用的IIC
 * @param - xfer: 传输结构体
 * @return 		: 传输结果,0 成功，其他值 失败;
 */
u8 i2c_master_transfer(I2C_Type *base, I2C_Transfer *xfer)
{
    unsigned char ret = 0;
	I2C_Direction direction = xfer->direction;	

	base->I2SR &= ~((1 << 1) | (1 << 4));			/* 清除标志位 */

	/* 等待传输完成 */
	while(!((base->I2SR >> 7) & 0X1)){}; 

	/* 如果是读的话，要先发送寄存器地址，所以要先将方向改为写 */
    if ((xfer->subaddressSize > 0) && (xfer->direction == kI2C_Read))
    {
        direction = kI2C_Write;
    }

	ret = i2c_master_start(base, xfer->slaveAddress, direction); /* 发送开始信号 */
    if(ret)
    {	
		return ret;
	}

	while(!(base->I2SR & (1 << 1))){};			/* 等待传输完成 */

    ret = i2c_check_and_clear_err(base, base->I2SR);	/* 检查是否出现传输错误 */
    if(ret)
    {
      	i2c_master_stop(base); 						/* 发送出错，发送停止信号 */
        return ret;
    }
	
    /* 发送寄存器地址 */
    if(xfer->subaddressSize)
    {
        do
        {
			base->I2SR &= ~(1 << 1);			/* 清除标志位 */
            xfer->subaddressSize--;				/* 地址长度减一 */
			
            base->I2DR =  ((xfer->subaddress) >> (8 * xfer->subaddressSize)); //向I2DR寄存器写入子地址
  
			while(!(base->I2SR & (1 << 1)));  	/* 等待传输完成 */

            /* 检查是否有错误发生 */
            ret = i2c_check_and_clear_err(base, base->I2SR);
            if(ret)
            {
             	i2c_master_stop(base); 				/* 发送停止信号 */
             	return ret;
            }  
        } while ((xfer->subaddressSize > 0) && (ret == I2C_STATUS_OK));

        if(xfer->direction == kI2C_Read) 		/* 读取数据 */
        {
            base->I2SR &= ~(1 << 1);			/* 清除中断挂起位 */
            i2c_master_repeated_start(base, xfer->slaveAddress, kI2C_Read); /* 发送重复开始信号和从机地址 */
    		while(!(base->I2SR & (1 << 1))){};/* 等待传输完成 */

            /* 检查是否有错误发生 */
			ret = i2c_check_and_clear_err(base, base->I2SR);
            if(ret)
            {
             	ret = I2C_STATUS_ADDRNAK;
                i2c_master_stop(base); 		/* 发送停止信号 */
                return ret;  
            }
           	          
        }
    }	


    /* 发送数据 */
    if ((xfer->direction == kI2C_Write) && (xfer->dataSize > 0))
    {
    	i2c_master_write(base, xfer->data, xfer->dataSize);
	}

    /* 读取数据 */
    if ((xfer->direction == kI2C_Read) && (xfer->dataSize > 0))
    {
       	i2c_master_read(base, xfer->data, xfer->dataSize);
	}
	return 0;
}
