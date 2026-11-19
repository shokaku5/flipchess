# 文件作用
黑白棋游戏 代码验证先行版

# 使用说明
## code1
核心代码，使用方法看注释
运行之前要先在终端（cmd）中输入
```
cd "d:\single chip\flipchess\output" # 这一行针对你的实际地址，写到项目结构的output文件夹下就可以了
chcp 65001
```
预期看到的输出
```
Active code page: 65001 
```
之后可以通过运行test.cpp测试输出的是不是中文。预期看到的结果是：
```
你好，世界！
欢迎学习C++!
Hello World!
```
此时运行code1.cpp即可看到正确的中文引导。

# 项目结构

```
.
├── README.md
├── code1.cpp                # 第一版可用的黑白棋游戏（功能未完全实现）
├── test.cpp                 # 测试中文是否能够输出在项目栏中
├── code1.exe                # 不用管
├── .vscode
│   ├── launch.json          # 不用管
│   └── tasks.json           # 不用管
└── output
    ├── code1.exe            # 不用管
    └── test.exe             # 不用管
```

# 历次更新说明
## 1.0.1
更新了悔棋机制，但是只能拿走那颗子，并且无法将上一步的影响复原。