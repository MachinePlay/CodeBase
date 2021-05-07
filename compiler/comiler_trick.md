# 编译顺序
-I 头文件
-L 库地址
-l 库名
基础库放在后面，因为编译过程是入栈过程，基础库放后面先进行处理，避免递归，加快编译速度


# 警告、优化常用选项
- W开启错误检查
- Wall 开启所有错误警告
- Werror 警告为错误 //开启后不能出现warning，编译过不去
- 建议开启 -W -Wall 目标是编译过程中不出现warning
- -O0, - 01,- 02/-o3 性能提升，追查不便
- g 增加调试信息
- fPIC 位置无关
- f{flag}特定优化
-funroll-loops 循环展开，循环多的情况下优化性能
-finline-functions简单函数自动内联

推荐参数
- g -pipe -Wall -W -fPIC
//-fPIC 可以删除


# 调试
其实不希望通过gdb调试来找问题，优雅的解决方案：
- 监控发现问题
- 打的日志定位问题
- 迫不得已出core，gdb定位疑难问题
常用命令
- 载入程序和core
- 流程控制
- 查看代码
- 断点
- 查看堆栈
- 多线程
- 杂项


## gdb
- 载入程序
gdb --args ./program arg1 arg2
- 在线调试
gdb -p pid //会把程序停掉
- 载入core
gdb -c core.pid ./program
gdb -c ./program core.pid
- gcore pid 在线生成core
主动获取程序core文件，且不会杀掉进程

- 流程控制
run
continue
next
step
finsh

- 查看

whatis 1+2.0
- 查看定义
ptype v
- 查看变量值
print /FMT v
p i
p (*arr)@10看数组前10

- 打印内存地址
x/(NUM)(FMT)addr
x/10c ptr， 看ptr开始连续10哥字符
x/10i main 打印main函数hoh10条指令
- 查看局部变量 info local
- 查看调用栈情况 info arg
- 查看栈调用 breakpoint(bt) bt full
- 换栈 frame(f) 1
- 多线程 info thread
- 切换线程 thread 1
- 查看寄存器信息 info reg
- 反汇编当前指令 disassemble
- 看变量变化情况 watch



# build system
## Makefile
由三个部分组成
- target 
- dependencies
- command

```
main:main.o lib1.o lib2.o
    g++ -o main.o lib1.o lib2.o
main.o:main.cpp
g++ -c main.cpp
```
生成DAG图

- 原理 dependencis修改时间比target新，则需要调用command
- PHONY 目标的command 都会执行

# Code Analysis Tool (valgrind)
- 内核和调试工具构成 memcheck
- helgrind 多线程竞争
- massif 堆栈问题

## 使用方法
- valgrind -tool=memcheck --leak-ckeck=full --log-file =valgring.log ./program
# Code Optimization 
gprof原理：
- 函数入口出口加入特殊代码
- 运行记录到文件
- gprof分析文件耗时比率

使用方法：
- g++ main.cpp -o main -pg
- ./main
- gprof -b main gmon.out

oprofile
- 事件采样和cpu时钟采样
- 粒度：代码/函数/程序
- 内核植入模块
- 哪些程序最耗时、哪些函数、哪段函数最耗时
组成：
- 内核模块
- 守护进程
- 控制脚本
- 分析工具


## 基础库
- 日志和配置
- 算法和数据结构
- URL、字符编码
- 网络协议
- 编程框架
- 其他
