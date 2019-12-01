CROSS_COMPILE ?= arm-linux-gnueabihf-
TARGET 	?= main

CC 		:= $(CROSS_COMPILE)gcc
LD 		:= $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

INCDIRS	:= 	imx6ul	\
			bsp/clk \
			bsp/led \
			bsp/key \
			bsp/beep \
			bsp/gpio \
			bsp/exit \
			bsp/int \
			bsp/delay

SRCDIRS := 	project	\
			bsp/clk \
			bsp/led \
			bsp/key \
			bsp/beep \
			bsp/gpio \
			bsp/exit \
			bsp/int \
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
	$(LD) -Timx6ul.lds -o $(TARGET).elf $^
	$(OBJCOPY) -O binary -S $(TARGET).elf $@
	$(OBJDUMP) -D -m arm $(TARGET).elf > $(TARGET).dis


$(SOBJS) : obj/%.o : %.s
	$(CC) -Wall -nostdlib -c -O2 $(INCLUDE) -o $@ $^

$(COBJS) : obj/%.o : %.c
	$(CC) -Wall -nostdlib -c -O2 $(INCLUDE) -o $@ $^

.PHONY : clean

clean :
	rm -rf $(TARGET).bin $(TARGET).dis $(TARGET).elf $(OBJS)



print :
	@echo INCLUDE = $(INCLUDE)
	@echo SFILES = $(SFILES)
	@echo CFILES = $(CFILES)

	@echo SFILE_NODIR = $(SFILE_NODIR)
	@echo CFILE_NODIR = $(CFILE_NODIR)
	@echo SOBJS = $(SOBJS)
	@echo COBJS = $(COBJS)

