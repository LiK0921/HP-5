CROSS_COMPILE ?= arm-linux-gnueabihf-

NAME ?= main

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump


OBJS := start.o	main.o

$(NAME).bin : $(OBJS)

#$^ 表示所有依赖文件，是一个集合
#编写了链接脚本文件imx6ul.lds，里面手动将start.o放在main.o的前面，则编译器就不会优化这个了
	$(LD) -Timx6ul.lds -o $(NAME).elf $^

#$@ 表示所有目标文件，是一个集合
	$(OBJCOPY) -O binary -S -g $(NAME).elf $@
	$(OBJDUMP) -D -m arm $(NAME).elf > $(NAME).dis

%.o : %.s
#先前带了参数 -O2   所以编译器自动优化，将main函数链接到了0X87800000，即使objs中start放在前面
	$(CC) -Wall -nostdlib -c -O2 -o $@ $<
%.o : %.S
	$(CC) -Wall -nostdlib -c -O2 -o $@ $<
%.o : %.c
	$(CC) -Wall -nostdlib -c -O2 -o $@ $<

.PHONY : clean
clean :
	rm *.o main.bin main.elf main.dis




