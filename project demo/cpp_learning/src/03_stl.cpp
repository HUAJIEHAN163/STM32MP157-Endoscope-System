/**
 * 03_stl.cpp — STL 容器
 *
 * STL（Standard Template Library）是 C++ 的标准容器库
 * 项目中用到的：
 *   vector  — 存储缓冲区列表、滤镜列表
 *   string  — 文件路径、设备名
 *   map     — 配置参数
 *
 * 编译：g++ 03_stl.cpp -o 03_stl -std=c++11 && ./03_stl
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

// ============================================================
// 1. vector — 动态数组（最常用）
// ============================================================

void demo_vector() {
    cout << "===== vector =====" << endl;

    // 创建
    vector<int> nums;                    // 空的
    vector<int> nums2 = {1, 2, 3, 4};   // 初始化列表
    vector<int> nums3(10, 0);            // 10 个 0

    // ⭐ 添加元素（项目中：往缓冲区列表添加新缓冲区）
    nums.push_back(10);
    nums.push_back(20);
    nums.push_back(30);

    // ⭐ 访问元素
    cout << "第一个: " << nums[0] << endl;       // 下标访问（不检查越界）
    cout << "第二个: " << nums.at(1) << endl;    // at 访问（越界会抛异常）
    cout << "最后一个: " << nums.back() << endl;

    // ⭐ 大小
    cout << "元素个数: " << nums.size() << endl;
    cout << "是否为空: " << nums.empty() << endl;

    // ⭐ 遍历方式一：下标（C 风格）
    cout << "下标遍历: ";
    for (int i = 0; i < (int)nums.size(); i++) {
        cout << nums[i] << " ";
    }
    cout << endl;

    // ⭐ 遍历方式二：范围 for（C++11，推荐）
    cout << "范围for: ";
    for (int n : nums) {
        cout << n << " ";
    }
    cout << endl;

    // ⭐ 遍历方式三：引用遍历（可以修改元素）
    for (int &n : nums) {
        n *= 2;  // 每个元素翻倍
    }
    cout << "翻倍后: ";
    for (int n : nums) cout << n << " ";
    cout << endl;

    // 删除
    nums.pop_back();   // 删除最后一个
    nums.erase(nums.begin());  // 删除第一个
    nums.clear();      // 清空

    // ⭐ 项目中的实际用法
    // vector<FrameBuffer> m_buffers;  — V4L2 缓冲区列表
    // vector<unsigned char> rgbBuf(width * height * 3);  — RGB 数据缓冲区

    // vector 作为动态数组，替代 C 的 malloc/free
    vector<unsigned char> buffer(640 * 480 * 2);  // 分配 YUYV 缓冲区
    cout << "缓冲区大小: " << buffer.size() << " 字节" << endl;
    // buffer.data() 返回底层数组指针，可以传给 C 函数
    unsigned char *ptr = buffer.data();
    ptr[0] = 128;
    cout << "buffer[0] = " << (int)buffer[0] << endl;
}

/*
常用方法总结：
v.push_back(x);      // 尾部添加元素
v.pop_back();        // 删除尾部元素

v.size();            // 元素个数
v.empty();           // 是否为空

v[0];                // 下标访问，不检查越界
v.at(i);             // 函数访问，会检查越界

v.front();           // 第一个元素（引用）
v.back();            // 最后一个元素（引用）

v.begin();           // 指向第一个元素的迭代器
v.end();             // 指向最后一个元素**下一个位置**的迭代器

v.clear();           // 清空所有元素
v.erase(迭代器);     // 删除某个位置元素
*/

/*三种遍历方法
// 1. 普通 for
for(int i=0; i<v.size(); i++)
    cout << v[i];

// 2. 范围 for（最简单）
for(auto x : v)
    cout << x;

// 3. 迭代器
for(auto it=v.begin(); it!=v.end(); it++)
    cout << *it;
*/
// ============================================================
// 2. string — 字符串（替代 C 的 char*）
// ============================================================

void demo_string() {
    cout << "\n===== string =====" << endl;

    // 创建
    string s1 = "hello";
    string s2("world");
    string s3 = s1 + " " + s2;  // 拼接（C 的 char* 不能直接 +）

    cout << s3 << endl;
    cout << "长度: " << s3.length() << endl;

    // ⭐ 常用操作
    cout << "子串: " << s3.substr(0, 5) << endl;          // "hello"
    cout << "查找: " << s3.find("world") << endl;          // 6
    cout << "包含: " << (s3.find("xyz") == string::npos ? "否" : "是") << endl;

    // ⭐ 与 C 字符串互转（项目中经常用到）
    const char *cstr = s3.c_str();   // string → const char*
    string s4(cstr);                  // const char* → string
    cout << "C字符串: " << cstr << endl;

    // ⭐ 数字转字符串
    int width = 640, height = 480;
    string resolution = to_string(width) + "x" + to_string(height);
    cout << "分辨率: " << resolution << endl;

    // ⭐ 项目中的实际用法
    string device = "/dev/video0";
    string filename = "capture_" + to_string(20250331) + ".jpg";
    cout << "设备: " << device << endl;
    cout << "文件名: " << filename << endl;
}

/*其他用法：
访问字符：
s[0]        // 第 0 个字符（和数组一样）
s.at(0)     // 带越界检查
s.front()   // 第一个字符
s.back()    // 最后一个字符

长度 / 空判断：
s.size()    // 长度（常用）
s.length()  // 和 size() 一样
s.empty()   // 是否为空，true/false

增删拼接：
s += "abc";             // 尾部追加
s.append("123");        // 追加
s.insert(2, "xx");      // 在位置2插入
s.erase(2, 3);          // 从位置2删3个字符
s.clear();              // 清空

查找与替换：
s.find("abc");          // 找子串，返回下标，找不到返回 string::npos
s.rfind("abc");         // 反向找
s.replace(0,2,"ab");    // 替换

比较：
if(s1 == s2) {}
if(s1 > s2) {}
// 直接用 > < == != 比较
*/
// ============================================================
// 3. map — 键值对（字典）
// map = 键值对容器（key-value），也叫字典、映射
// ============================================================

void demo_map() {
    cout << "\n===== map =====" << endl;

    // 创建
    map<string, int> config;

    // ⭐ 插入
    config["width"] = 640;
    config["height"] = 480;
    config["fps"] = 30;
    config["brightness"] = 128;
    config.insert({"contrast", 64});  // 另一种插入方式

    // ⭐ 访问
    cout << "宽度: " << config["width"] << endl;
    cout << "帧率: " << config["fps"] << endl;

    // ⭐ 检查 key 是否存在
    if (config.find("exposure") != config.end()) {
        cout << "曝光: " << config["exposure"] << endl;
    } else {
        cout << "曝光: 未设置" << endl;
    }

    // C++11 更简洁的检查方式
    if (config.count("fps") > 0) {
        cout << "fps 存在" << endl;
    }

    // ⭐ 遍历
    cout << "所有配置:" << endl;
    for (const auto &pair : config) {
        cout << "  " << pair.first << " = " << pair.second << endl;
    }

    // ⭐ 大小和删除
    cout << "配置项数: " << config.size() << endl;
    config.erase("brightness");
    cout << "删除后: " << config.size() << endl;
    /*
    m.erase(1);        // 按key删

    auto it = m.find(2);
    m.erase(it);       // 按迭代器删

    m.clear();         // 清空
    */
}

/*函数指针的用法

#include <iostream>
#include <map>
#include <string>
using namespace std;

// 1. 定义函数类型（返回void，无参数）
typedef void (*FuncPtr)();

// 测试函数
void func1() { cout << "执行函数1\n"; }
void func2() { cout << "执行函数2\n"; }

int main() {
    // 2. 创建 map：key = string，value = 函数指针
    map<string, FuncPtr> funcMap;

    // 3. 放入函数
    funcMap["open"] = func1;
    funcMap["close"] = func2;

    // 4. 查找并调用
    auto it = funcMap.find("open");
    if (it != funcMap.end()) {
        it->second();  // 调用函数！
    }

    return 0;
}
*/

/*C中的实现

#include <stdio.h>
#include <string.h>

// 1. 定义函数指针类型
typedef void (*FuncPtr)();

// 2. 定义“键值对”结构体（模拟 map 元素）
typedef struct {
    const char* key;   // 键：命令名
    FuncPtr func;      // 值：函数指针
} Command;

// 3. 写几个函数
void open()  { printf("打开设备\n"); }
void close() { printf("关闭设备\n"); }
void reset() { printf("重启设备\n"); }

// 4. 构建“命令表”（就是 C 语言版 map）
Command cmdTable[] = {
    {"open",  open},
    {"close", close},
    {"reset", reset},
};

// 计算表大小
#define TABLE_SIZE (sizeof(cmdTable)/sizeof(Command))

// 5. 查找函数（模拟 map.find）
FuncPtr findCommand(const char* key) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(cmdTable[i].key, key) == 0) {
            return cmdTable[i].func; // 找到，返回函数指针
        }
    }
    return NULL; // 没找到
}

// 测试
int main() {
    // 查找并执行
    FuncPtr func = findCommand("open");
    if (func) {
        func();
    }

    func = findCommand("close");
    if (func) {
        func();
    }

    return 0;
}
*/

// ============================================================
// 4. auto 关键字（C++11）— 自动类型推导
// ============================================================

void demo_auto() {
    cout << "\n===== auto =====" << endl;

    // auto 让编译器自动推导类型，避免写很长的类型名
    auto x = 42;           // int
    auto pi = 3.14;        // double
    auto name = string("hello");  // string

    vector<int> nums = {1, 2, 3, 4, 5};

    // 没有 auto：
    // for (vector<int>::iterator it = nums.begin(); it != nums.end(); ++it)

    // 有 auto（简洁多了）：
    for (auto it = nums.begin(); it != nums.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;

    // 最常用：范围 for + auto
    map<string, int> config = {{"width", 640}, {"height", 480}};
    for (const auto &pair : config) {
        cout << pair.first << "=" << pair.second << " ";
    }
    cout << endl;
}


// ============================================================
// 5. 引用（&）— C++ 中替代指针的安全方式
// ============================================================

void demo_reference() {
    cout << "\n===== 引用 =====" << endl;

    int a = 10;
    int &ref = a;   // ref 是 a 的别名，不是拷贝
    ref = 20;
    cout << "a = " << a << endl;  // 20（通过 ref 修改了 a）

    // ⭐ 函数参数用引用：避免拷贝大对象
    // C 风格：void process(ImageBuffer *buf)  — 传指针
    // C++ 风格：void process(ImageBuffer &buf) — 传引用（更安全，不会是 NULL）

    // ⭐ const 引用：只读，不能修改（最常用）
    // void display(const QImage &image)  — 只读取图像，不修改
    // 项目中到处都是 const 引用参数

    string longString = "这是一个很长的字符串，如果按值传递会拷贝一份";

    // 按值传递：拷贝整个字符串（慢）
    // void func(string s) { ... }

    // 按 const 引用传递：不拷贝，只读（快）
    // void func(const string &s) { ... }

    auto printByValue = [](string s) { cout << "值: " << s.length() << endl; };
    auto printByRef = [](const string &s) { cout << "引用: " << s.length() << endl; };

    printByValue(longString);  // 拷贝了一份
    printByRef(longString);    // 没有拷贝
}


int main() {
    demo_vector();
    demo_string();
    demo_map();
    demo_auto();
    demo_reference();
    return 0;
}

/*
附录：
C++ 中 冒号 : 的 5 种核心用法（全覆盖）
1️⃣ 范围 for 循环（你现在用的）
cpp
运行
for (auto x : v) 
含义： in（在… 里面）作用： 遍历容器 / 数组
2️⃣ 构造函数 初始化列表（你之前学的）
cpp
运行
Camera() : m_width(640), m_height(480) {}
含义： 给成员变量赋值作用： 高效初始化类成员
3️⃣ 类 继承
cpp
运行
class Student : public Person {}
含义： 继承自作用： 表示子类继承父类
4️⃣ 访问修饰符（public/private 后面）
cpp
运行
public:
    void fun();
含义： 下面的内容属于 public作用： 划分权限区域
5️⃣ 位域 / 三目运算符（少见）
cpp
运行
// 三目运算符
int a = b > c ? 1 : 0;

// 位域（嵌入式用）
struct { int a : 3; };


:: 叫 作用域解析运算符（Scope Resolution Operator）简单说：指明 “这个东西来自哪里”作用只有 3 种，全是你日常必用的。
1. 访问命名空间里的东西（最常用）
cpp
运行
std::string
std::vector
std::cout
std::map
std 是 C++ 标准库的命名空间
:: 表示：我要使用 std 里面的 xxx
如果你没写 using namespace std;就必须这样写：
cpp
运行
std::string s = "hello";
std::cout << s << std::endl;
2. 访问类里面的静态成员 / 函数
cpp
运行
class Camera {
public:
    static int count;  // 静态变量
    static void show();// 静态函数
};
使用时：
cpp
运行
Camera::count = 10;    // 访问静态变量
Camera::show();        // 调用静态函数
:: 表示：我要调用 Camera 类里的静态成员
3. 在类外面写成员函数（必须用）
这是你写代码一定会遇到的！
cpp
运行
class Camera {
public:
    void open();  // 声明
};
在类外面实现函数：
cpp
运行
// 必须写 Camera:: 告诉编译器这是类的成员函数
void Camera::open() {
    // ...
}
没有：：就会编译报错！
4. 解决同名冲突（区分全局 / 局部）
cpp
运行
int val = 100;  // 全局变量

void test() {
    int val = 10;

    cout << val;      // 局部变量 10
    cout << ::val;    // 全局变量 100（::前面空着=全局作用域）
}
超简总结（背这 3 条就够）
命名空间用：std::cout
类静态成员用：Camera::open()
类外实现函数必须用：void Camera::open() {}

一句话记住
:: = “属于哪里”指明变量、函数、类来自哪个命名空间 / 哪个类。
*/