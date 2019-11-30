objs := start.o	main.o
main.bin : $(objs)

#用-Ttext 0X87800000来指定链接地址
#这样的话所有的文件都会链接到以 0X87800000 为起始地址的区域。但是有时候我们很多文件需要链接到指定的区域，或者叫做段里面
#要完成这个功能我们就需要使用到链接脚本用于描述文件应该如何被链接在一起形成最终的可执行文件。其主要目
#的是描述输入文件中的段如何被映射到输出文件中，并且控制输出文件中的内存排布
#	arm-linux-gnueabihf-ld -Ttext 0X87800000 -o main.elf $^
#$^ 表示所有依赖文件，是一个集合

#编写了链接脚本文件imx6ul.lds，里面手动将start.o放在main.o的前面，则编译器就不会优化这个了
	arm-linux-gnueabihf-ld -Timx6ul.lds -o main.elf $^

#$@ 表示所有目标文件，是一个集合
	arm-linux-gnueabihf-objcopy -O binary -S -g main.elf $@
	arm-linux-gnueabihf-objdump -D -m arm main.elf > main.dis

%.o : %.s
#先前带了参数 -O2   所以编译器自动优化，将main函数链接到了0X87800000，即使objs中start放在前面
	arm-linux-gnueabihf-gcc -Wall -nostdlib -c -O2 -o $@ $<
#$< 表示所有依赖文件中的第一个文件
#	arm-linux-gnueabihf-gcc -Wall -nostdlib -c -o $@ $<
%.o : %.S
	arm-linux-gnueabihf-gcc -Wall -nostdlib -c -O2 -o $@ $<
#	arm-linux-gnueabihf-gcc -Wall -nostdlib -c -o $@ $<
%.o : %.c
	arm-linux-gnueabihf-gcc -Wall -nostdlib -c -O2 -o $@ $<
#	arm-linux-gnueabihf-gcc -Wall -nostdlib -c -o $@ $<

.PHONY : clean
clean :
	rm *.o main.bin main.elf main.dis




