## 第3章 嵌入式编程基础知识
### 3.1 交叉编译工具
要编译出能在ARM平台上运行的程序：arm-linux-gcc,arm-linux-ld  
1. arm-linux-gcc  
gcc 与 g++ 分别是 gnu 的 c & c++ 编译器  

gcc/g++ 在执行编译工作的时候，总共需要4步：  
（1）预处理,生成 .i 的文件[预处理器cpp]  
（2）编译，将预处理后的文件转换成汇编语言, 生成文件 .s [编译器egcs]  
（3）汇编，有汇编变为目标代码(机器代码)生成 .o 的文件[汇编器as]  
（4）连接，连接目标代码, 生成可执行程序 [链接器ld]  

[GCC参数详解](https://www.runoob.com/w3cnote/gcc-parameter-detail.html)  

### 3.2 Makefile
#### [Makefile](https://my.oschina.net/u/1413984/blog/199029)  
有了这个Makefile文件，不论我们什么时候修改了源程序当中的什么文件，我们只要执行make命令，我们的编译器都只会去编译和我们修改的文件有关的文件，其它的文件它连理都不想去理的。   
下面我们学习Makefile是如何编写的。   
在Makefile中也#开始的行都是注释行.Makefile中最重要的是描述文件的依赖关系的说明。一般的格式是：   
target：components  
TAB rule   
第一行表示的是依赖关系。第二行是规则。

```
# 这是上面那个程序的Makefile文件: 
main：main.o mytool1.o mytool2.o 
  gcc -o main main.o mytool1.o mytool2.o 
main.o：main.c mytool1.h mytool2.h 
  gcc -c main.c 
mytool1.o：mytool1.c mytool1.h 
  gcc -c mytool1.c 
mytool2.o：mytool2.c mytool2.h 
  gcc -c mytool2.c
```
比如说我们上面的那个Makefile文件的第二行。   
main：main.o mytool1.o mytool2.o   
表示我们的目标(target)main的依赖对象(components)是main.o mytool1.omytool2.o 当倚赖的对象在目标修改后修改的话，就要去执行规则一行所指定的命令。就象我们的上面那个Makefile第三行所说的一样要执行 gcc-o main main.o mytool1.o mytool2.o 注意规则一行中的TAB表示那里是一个TAB键   

Makefile有三个非常有用的变量。分别是$@，$^，$<代表的意义分别是：   
$@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。   
如果我们使用上面三个变量，那么我们可以简化我们的Makefile文件为：   
```
# 这是简化后的Makefile 
main：main.o mytool1.o mytool2.o 
  gcc -o $@ $^ 
main.o：main.c mytool1.h mytool2.h 
  gcc -c $< 
mytool1.o：mytool1.c mytool1.h 
  gcc -c $< 
mytool2.o：mytool2.c mytool2.h 
  gcc -c $< 
```

经过简化后,我们的Makefile是简单了一点，不过人们有时候还想简单一点。这里我们学习一个Makefile的缺省规则   
.c.o：   
gcc -c $<   
这个规则表示所有的 .o文件都是依赖与相应的.c文件的。例如mytool.o依赖于mytool.c这样Makefile还可以变为：   
```
# 这是再一次简化后的Makefile 
main：main.o mytool1.o mytool2.o 
  gcc -o $@ $^ 
.c.o： 
  gcc -c $< 
```

[对 makefile 中 .c.o 的理解](https://www.cnblogs.com/gaojian/archive/2012/09/20/2695503.html)  
```
LIBS=gao.o
all: $(LIBS)　　
  @echo "final"
.c.o:　　
  gcc -o $@ $<　　
  echo "in .c.o rule"
```
执行 结果：  
gcc -o gao.o gao.c  
in .c.o rule  
final  
注意其中的 .c.o ，其实是 和 %o:%c 等价。是一个旧格式。  
所以 $@ 对应了 gao.o ，$< 对应了 gao.c ，不要弄反了。  

#### [跟我一起写Makefile 1.0](https://seisman.github.io/how-to-write-makefile/overview.html)

编译时，编译器需要的是语法的正确，函数与变量的声明的正确。对于后者，通常是你需要告诉编译器头文件的所在位置（**头文件中应该只是声明，而定义应该放在C/C++文件中**），只要所有的语法正确，编译器就可以编译出中间目标文件。一般来说，每个源文件都应该对应于一个中间目标文件（ .o 文件或 .obj 文件）。  


#### Makefile实例
```
#longan system
#CC:=/home/username/workspace/lichee/longan/out/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
#CC:=/home/username/workspace/lichee/longan/out/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-gcc
#tina system
#CC:=/home/username/workspace/tina/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-muslgnueabi-gcc-6.4.1

#longan system
CC := ../../../../../../../../out/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-gcc
ARCH_DIR := ../../../../../linux-5.4/arch/arm/include/
CFLAGS := -I $(ARCH_DIR)
TARGET := csi_test_mplane

.PHONY: all clean

all: $(TARGET)

csi_test_mplane: csi_test_mplane.c
	$(CC) $(CFLAGS) -static  $^  -o  $@ -lc -lgcc_eh

	#mv csi_test_mplane ~/to_server/camera_test/

clean:
	rm -rf $(TARGET)

```
1. [](https://blog.csdn.net/newchenxf/article/details/51743181)  
当gcc -static 用于编译一个程序时，会使此程序静态编译（把动态库的函数和所依赖的任何的东西，都编译进本程序），编译好后，文件会非常大，但是，运行时就不需要依赖任何动态库。  
2. [gcc -I -i -L -l 参数区别 / -l(静态库/动态库)](https://blog.csdn.net/abcdu1/article/details/86083295)  
-I ( i 的大写)  ：指定头文件路径（相对路径或绝对路径，建议相对路径）  
3. [-lz -lrt -lm -lc都是什么库](https://blog.csdn.net/haifengid/article/details/51732011)  
4. 



## 第5章 GPIO接口

嵌入式开发步骤：编程→编译→烧写程序→运行  













