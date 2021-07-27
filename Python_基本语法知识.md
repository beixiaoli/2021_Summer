## Python

>[Python3|菜鸟教程](https://www.runoob.com/python3/python3-tutorial.html)

### 简介

解释型、交互式、面向对象

### 环境搭建

`python -v` ：查看版本  

运行方法：  
1. 交互式解释器：
你可以通过命令行窗口进入python并开在交互式解释器中开始编写Python代码。  
2. 命令行脚本：  
在你的应用程序中通过引入解释器可以在命令行中执行Python脚本。  
3. 集成开发环境（IDE：Integrated Development Environment）: PyCharm  

### 基础语法

注释：#  
缩进：表示代码块  
多行：\  
转义符：\
取消转义：r  
截取字符串：变量[头下标:尾下标:步长]  
空行：分隔两段不同功能或含义的代码，便于日后代码的维护或重构。  
不换行：`end=""`  
导入模块：`import module` `from module import function`  
`-h`：参数帮助信息  

### 基本数据类型

标准数据类型：  
Number（数字）  
String（字符串''）不可修改  
List（列表[]）可以修改/有序  
Tuple（元组()）不可修改  
Set（集合{}）  
Dictionary（字典{}）无序  

查询变量所指的对象类型：`type()` `isinstance`  
创建对象：赋值  
删除对象：`del` 
除法：`/`返回一个浮点数，`//`返回一个整数  
字符串：`+`连接，`*`复制  

### 解释器

设置环境变量  
交互式编程  
脚本式编程  

### 注释

单行：#  
多行：'''/"""

### 运算符

算术运算符：除/，取模%，幂**，向下取整除//  
比较（关系）运算符  
赋值运算符  
逻辑运算符：与and或or非not  
位运算符：与&或|异或^取反~左移<<右移>>  
成员运算符：in/not in  
身份运算符：is/not is  
运算符优先级  
[Python3运算符优先级](https://www.runoob.com/python3/python3-basic-operators.html#ysf4)  

### 数字(Number)

整型(int)：不限制大小，可当作Long  
浮点型(float)  
复数(complex)  

最后被输出的表达式结果被赋值给变量 _  

### 字符串

截取字符串**左闭右开**：变量[头下标:尾下标]  
[转义字符](https://www.runoob.com/python3/python3-string.html)  
`r` `R`：原始字符串  
格式化字符串：  
`%s`：字符串格式符  
```
#!/usr/bin/python3
print ("我叫 %s 今年 %d 岁!" % ('小明', 10))
```
f-string字面量格式化字符串：  
`f'Hello {name}'`  

### 列表

可更新  
`append()`：添加列表项  
`del list[2]`：删除列表项  
`len(list)`：元素个数  
`max(list)`：元素最大值  
`min(list)`：元素最小值  
`list(tup)`：把元组转换为列表  

### 元组

元素不能修改  
元组中只包含一个元素时，需要在元素后面添加逗号`,`，否则括号会被当作运算符使用  
`del tup`：删除整个元组  
`len(tuple)`：元素个数  
`max(tuple)`：元素最大值  
`min(tuple)`：元素最小值  
`tuple(list)`：把列表转换为元组  
元组的不可变指的是元组所指向的内存中的内容不可变  

### 字典

可变  
键唯一，值不必  
`del dict['']`：删除键  
`dict.clear()`：清空字典  
`len(dict)`：元素个数  
`str(dict)`：以字符串表示输出字典  
`type(variable)`：返回输入的变量类型  

### 集合

无序的不重复元素序列  
`{}` `set()`：创建集合  
`s.add()` `s.update()`：添加元素  
`s.remove()` `s.discard()`：移除元素  
`s.pop()`：随机删除集合中的一个元素  
`len(s)`：元素个数  
`s.clear()`：清空集合  


