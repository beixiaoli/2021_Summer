## Python

>[Python3|菜鸟教程](https://www.runoob.com/python3/python3-tutorial.html)

### 第一步

复合赋值：`a,b=0,1`  
`end=' '`：将结果输出到同一行  

### 条件控制

if语句：  
```
if condition_1:
    statement_block_1
elif condition_2:
    statement_block_2
else:
    statement_block_3
```

### 循环语句

while语句：  
```
while <expr>:
    <statement(s)>
else:
    <additional_statement(s)>
```
for语句：  
```
for <variable> in <sequence>:
    <statements>
else:
    <statements>
```
跳出循环：`break`  
下一轮循环：`continue`  
遍历：`range(开始,结尾,步长)`  
遍历序列：`range(len(a))`  
创建列表：`list(range(5))`  
占位语句：`pass`  

### 迭代器与生成器

创建迭代器对象：`iter()`  
迭代器下一个元素：`next()`  
用于标识迭代的完成：`StopIteration`异常  

生成器：使用了`yield`的函数，返回迭代器  
每次遇到 yield 时函数会暂停并保存当前所有的运行信息，返回 yield 的值, 并在下一次执行 next() 方法时从当前位置继续运行。  

### 函数

定义：`def func(a,b):`  
不可变类型：字符串，元组，数字——值传递    
可变类型：列表，集合，字典——引用传递    

### 数据类型

列表：  
```
a=[1,2,3,4,5,6]
print('1的数量:',a.count(1))
a.append(7)
print('a:',a)
a.insert(3,0)
print('a:',a)
b=[8,9]
a.extend(b)
print('a:',a)
a.remove(6)
print('a:',a)
a.pop()
print('a:',a)
print('7的索引:',a.index(7))
a.sort()
print('a排序:',a)
a.reverse()
print('a倒排:',a)
```

```
1的数量: 1
a: [1, 2, 3, 4, 5, 6, 7]
a: [1, 2, 3, 0, 4, 5, 6, 7]
a: [1, 2, 3, 0, 4, 5, 6, 7, 8, 9]
a: [1, 2, 3, 0, 4, 5, 7, 8, 9]
a: [1, 2, 3, 0, 4, 5, 7, 8]
7的索引: 6
a排序: [0, 1, 2, 3, 4, 5, 7, 8]
a倒排: [8, 7, 5, 4, 3, 2, 1, 0]
```

将列表当做堆栈使用：后进先出`append()` `pop()`  
将列表当做队列使用：先进先出  

列表推导式：  
```
a=[1,2]
b=[4,5,6]
print([x*y for x in a if x>1 for y in b])
print([a[i]*b[i] for i in range(len(a))])
```

```
[8, 10, 12]
[4, 10]
```

元组:由若干逗号分隔的值组成  

集合:一个无序不重复元素的集，支持推导式  

字典：无序的键-值对集合  
序列是以连续的整数为索引，字典以关键字为索引（字符串或数值）  
```
#{}大括号创建字典：
student={'AA':233,'AB':435,'AK':769}
print(student)
#从键值对元组列表中构建字典：
print(dict([('BB',78),('BJ','00')]))
#字典推导可以用来创建任意键和值的表达式词典：
print({x:x**2 for x in range(1,5)})
#如果关键字只是简单的字符串，使用关键字参数指定键值对有时候更方便：
print(dict(CC=99,CD=66))
```

```
{'AA': 233, 'AB': 435, 'AK': 769}
{'BB': 78, 'BJ': '00'}
{1: 1, 2: 4, 3: 9, 4: 16}
{'CC': 99, 'CD': 66}
```

遍历：  
```
#在字典中遍历时，关键字和对应的值可以使用 items() 方法同时解读出来：
knights = {'gallahad': 'the pure', 'robin': 'the brave'}
for k, v in knights.items():
	print(k, v)
	
#在序列中遍历时，索引位置和对应值可以使用 enumerate() 函数同时得到：
for i, v in enumerate(['tic', 'tac', 'toe']):
	print(i, v)
	
#同时遍历两个或更多的序列，可以使用 zip() 组合：
questions = ['name', 'quest', 'favorite color']
answers = ['lancelot', 'the holy grail', 'blue']
for q, a in zip(questions, answers):
	print('What is your {0}?  It is {1}.'.format(q, a))
```

```
gallahad the pure
robin the brave
0 tic
1 tac
2 toe
What is your name?  It is lancelot.
What is your quest?  It is the holy grail.
What is your favorite color?  It is blue.
```

反向遍历：`reversed`  
顺序遍历：`sorted`  

### 模块

模块是一个包含所有你定义的函数和变量的文件，其后缀名是.py。  

1. import sys 引入 python 标准库中的 sys.py 模块；这是引入某一模块的方法。  
2. sys.argv 是一个包含命令行参数的列表。   
3. sys.path 包含了一个 Python 解释器自动查找所需模块的路径的列表。  

包是一种管理 Python 模块命名空间的形式，采用"点模块名称"。  

### 输入输出

①表达式语句  
②`print()`  
③`write()`  
④`str.format()`  
⑤转成字符串：`repr()` `str()`  

### 文件
`open(file, mode='r', buffering=-1, encoding=None, errors=None, newline=None, closefd=True, opener=None)`  

### 错误和异常

1. 异常捕捉：  
![异常捕捉](https://www.runoob.com/wp-content/uploads/2019/07/try_except_else_finally.png)  
2. 抛出异常：  
![触发异常](https://www.runoob.com/wp-content/uploads/2019/07/raise.png)  

### 面向对象 

1. 类(Class): 用来描述具有相同的属性和方法的对象的集合。它定义了该集合中每个对象所共有的属性和方法。对象是类的实例。  
2. 方法：类中定义的函数。  
3. 类变量：类变量在整个实例化的对象中是公用的。类变量定义在类中且在函数体之外。类变量通常不作为实例变量使用。  
4. 数据成员：类变量或者实例变量用于处理类及其实例对象的相关的数据。  
5. 实例变量：在类的声明中，属性是用变量来表示的，这种变量就称为实例变量，实例变量就是一个用 self 修饰的变量。  
6. 继承：即一个派生类（derived class）继承基类（base class）的字段和方法。继承也允许把一个派生类的对象作为一个基类对象对待。例如，有这样一个设计：一个Dog类型的对象派生自Animal类，这是模拟"是一个（is-a）"关系（例图，Dog是一个Animal）。  
7. 实例化：创建一个类的实例，类的具体对象。  
8. 对象：通过类定义的数据结构实例。对象包括两个数据成员（类变量和实例变量）和方法。  

### 标准库
>[Python3标准库](https://www.runoob.com/python3/python3-stdlib.html)  

### 实例
>[Python3实例](https://www.runoob.com/python3/python3-examples.html)
