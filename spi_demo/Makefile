CROSS_COMPILE ?= arm-linux-gnueabihf-
TARGET 	?= main

CC 		:= $(CROSS_COMPILE)gcc
LD 		:= $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

#因为计算波特率使用到除法运算，要使用到数学库，所以要链接库目录
LIBPATH := -lgcc -L /usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/4.9.4


INCDIRS	:= 	imx6ul	\
			bsp/clk \
			bsp/led \
			bsp/key \
			bsp/beep \
			bsp/gpio \
			bsp/exit \
			bsp/int \
			bsp/epit\
			bsp/gpt \
			bsp/uart \
			bsp/lcd \
			bsp/rtc \
			bsp/i2c \
			bsp/ap3216c \
			bsp/spi     \
			bsp/icm20608 \
			stdio/include \
			bsp/delay

SRCDIRS := 	project	\
			bsp/clk \
			bsp/led \
			bsp/key \
			bsp/beep \
			bsp/gpio \
			bsp/exit \
			bsp/int \
			bsp/epit\
			bsp/gpt \
			bsp/uart \
			bsp/lcd \
			bsp/rtc \
			bsp/i2c \
			bsp/ap3216c \
			bsp/spi     \
			bsp/icm20608 \
			stdio/lib \
			bsp/delay

#patsubst是一个函数，搜索并替换字符串
INCLUDE := 	$(patsubst %, -I %, $(INCDIRS))

#wildcard是通配符函数
SFILES	:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.s))
CFILES	:=	$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))



SFILE_NODIR	:= $(notdir $(SFILES))
CFILE_NODIR	:= $(notdir $(CFILES))

SOBJS		:= $(patsubst %, obj/%, $(SFILE_NODIR:.s=.o))
COBJS		:= $(patsubst %, obj/%, $(CFILE_NODIR:.c=.o))

OBJS		:= $(SOBJS)$(COBJS)

VPATH		:= $(SRCDIRS)

$(TARGET).bin : $(OBJS)
	$(LD) -Timx6ul.lds -o $(TARGET).elf $^ $(LIBPATH)
	$(OBJCOPY) -O binary -S $(TARGET).elf $@
	$(OBJDUMP) -D -m arm $(TARGET).elf > $(TARGET).dis


$(SOBJS) : obj/%.o : %.s
	$(CC) -Wall -nostdlib -fno-builtin -c -O2  $(INCLUDE) -o $@ $<

$(COBJS) : obj/%.o : %.c
	$(CC) -Wall -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard -Wa,-mimplicit-it=thumb -nostdlib -fno-builtin -c -O2  $(INCLUDE) -o $@ $<
.PHONY : clean

clean :
	rm -rf $(TARGET).bin $(TARGET).dis $(TARGET).elf $(OBJS) $(SOBJS)



print :
	@echo INCLUDE = $(INCLUDE)
	@echo SFILES = $(SFILES)
	@echo CFILES = $(CFILES)

	@echo SFILE_NODIR = $(SFILE_NODIR)
	@echo CFILE_NODIR = $(CFILE_NODIR)
	@echo SOBJS = $(SOBJS)
	@echo COBJS = $(COBJS)

