# C++ 嵌入式开发学习工程

> 面向有 C 语言基础的开发者，重点学习 OpenCV + Qt 嵌入式开发所需的 C++ 知识。
> 不追求全面，只学项目中用得到的。

## 文件列表

| 文件 | 内容 | 项目中的体现 |
|------|------|-------------|
| 01_class_basics.cpp | 类基础：struct→class、访问控制、构造/析构、this、const | V4l2Camera 类的基本结构 |
| 01_class_advanced.cpp | 类进阶：深拷贝、static、友元、运算符重载 | QImage 深拷贝、OneVideo::WIDTH |
| 02_inherit_basics.cpp | 继承基础：单继承、访问控制、构造顺序、方法覆盖 | QThread→V4l2Camera |
| 02_inherit_polymorphism.cpp | 多态：虚函数、override、纯虚函数、抽象类 | paintEvent/keyPressEvent 覆盖 |
| 03_stl.cpp | STL 容器：vector、string、map、auto、引用 | 缓冲区列表、配置参数、const引用传参 |
| 04_memory.cpp | 内存管理：RAII、unique_ptr、shared_ptr、Qt父子树 | V4l2Camera 资源释放、cv::Mat 引用计数 |
| 05_template.cpp | 模板基础：函数模板、类模板、看懂模板用法 | vector\<int\>、cv::Ptr\<CLAHE\> |
| 06_lambda.cpp | Lambda：语法、捕获列表、STL算法、模拟信号槽 | Qt connect 中的 lambda 回调 |
| 07_qt_basics.cpp | Qt 核心：信号槽、QThread、QImage、事件处理、控件 | 整个 GUI 框架（伪代码讲解） |
| 08_opencv_basics.cpp | OpenCV：Mat、颜色转换、CLAHE/锐化/降噪、IO | 图像处理管线 |

## 编译方式

```bash
# 基础 C++ 文件（01-06），任何 Linux/Mac 都能编译
g++ src/01_class_basics.cpp -o 01_class_basics -std=c++11 && ./01_class_basics
g++ src/01_class_advanced.cpp -o 01_class_advanced -std=c++11 && ./01_class_advanced
g++ src/02_inherit_basics.cpp -o 02_inherit_basics -std=c++11 && ./02_inherit_basics
g++ src/02_inherit_polymorphism.cpp -o 02_poly -std=c++11 && ./02_poly
g++ src/03_stl.cpp -o 03_stl -std=c++11 && ./03_stl
g++ src/04_memory.cpp -o 04_memory -std=c++11 && ./04_memory
g++ src/05_template.cpp -o 05_template -std=c++11 && ./05_template
g++ src/06_lambda.cpp -o 06_lambda -std=c++11 && ./06_lambda

# 07_qt_basics.cpp 是伪代码讲解，可以编译但只输出学习提示
g++ src/07_qt_basics.cpp -o 07_qt -std=c++11 && ./07_qt

# 08_opencv_basics.cpp 需要 OpenCV 环境
g++ src/08_opencv_basics.cpp -o 08_opencv $(pkg-config --cflags --libs opencv) -std=c++11 && ./08_opencv
```

## 学习建议

1. **按顺序学**：01 → 02 → 03 → 04 → 05 → 06 → 07 → 08
2. **动手运行**：每个文件编译运行，看输出，对照代码理解
3. **修改实验**：改改代码看看会发生什么（比如去掉 virtual、去掉 copy）
4. **重点标记**：代码中 ⭐ 标记的是项目中直接用到的知识点
5. **不求全懂**：先过一遍建立印象，写项目时遇到不懂的再回来查


重点文件文件：
01_class_basics	V4l2Camera、MainWindow、ImageProcessor 都是类
03_stl	vector 存缓冲区、string 存路径、引用传参到处都是
06_lambda	Qt 的 connect 几乎都用 lambda 写
07_qt_basics	信号槽、QThread、QImage 是项目骨架
08_opencv_basics	CLAHE、滤波、颜色转换是项目核心功能

了解即可（遇到时能看懂就行）：
01_class_advanced	深拷贝原理知道就行，项目中用 QImage::copy() 一行搞定；static/友元/运算符重载偶尔遇到
02_inherit_basics	知道"继承 QThread 覆盖 run()"这个模式就够了，不需要深入研究继承体系
02_inherit_polymorphism	知道 virtual + override 是什么意思就行，项目中只是覆盖 paintEvent/keyPressEvent
04_memory	知道 RAII 概念就行，项目中 Qt 父子树自动管理内存，很少手动用智能指针
05_template	最不需要深入的，只要能看懂 vector<int> 和 cv::Ptr<CLAHE> 的写法就够了

建议
01_basics、03、06、07、08 认真看，各花 15-20 分钟

其余的快速浏览 5 分钟，知道有这些概念，遇到时回来查

总共 2 小时左右，然后直接开始写代码
