/**
 * 01_class_basics.cpp — 类的基本语法
 *
 * 从 C 的 struct 过渡到 C++ 的 class
 * 重点：成员变量、成员函数、访问控制、对象的创建和使用
 *
 * 编译：g++ 01_class_basics.cpp -o 01_class_basics -std=c++11 && ./01_class_basics
 */

#include <iostream>
#include <string>
#include <cstring>
using namespace std;

// ============================================================
// 1. C 的 struct vs C++ 的 class
// ============================================================

// --- C 风格 ---
// 数据和操作是分开的，函数需要传入 struct 指针
struct Point_C {
    int x;
    int y;
};

void point_print_c(struct Point_C *p) {
    printf("C: (%d, %d)\n", p->x, p->y);
}

// --- C++ 风格 ---
// 数据和操作封装在一起，函数属于类
class Point {
public:
    int x;
    int y;

    void print() {
        // 不需要传指针，直接访问自己的成员
        cout << "C++: (" << x << ", " << y << ")" << endl;
    }
};


// ============================================================
// 2. 访问控制：public / private / protected
// ============================================================

class Student {
// private: 只有类内部的函数能访问（默认就是 private）
private:
    string m_name;   // m_ 前缀是命名习惯，表示 member（成员变量）
    int m_age;
    int m_score;

// public: 外部可以访问的接口
public:
    // 设置数据（写入）
    void setName(const string &name) { m_name = name; }
    void setAge(int age) {
        // 可以在这里加验证逻辑
        if (age >= 0 && age <= 150) {
            m_age = age;
        } else {
            cout << "年龄无效!" << endl;
        }
    }
    void setScore(int score) { m_score = score; }

    // 获取数据（读取）
    string getName() const { return m_name; }   // const 表示这个函数不会修改成员
    int getAge() const { return m_age; }
    int getScore() const { return m_score; }

    // 行为
    void introduce() const {
        cout << "我叫" << m_name << "，" << m_age << "岁，成绩" << m_score << endl;
    }
};

// 为什么用 private + get/set 而不是直接 public？
// 因为可以在 set 中加验证（比如年龄不能为负数）
// 项目中：V4l2Camera 的 m_fd 是 private，外部不能随意修改文件描述符


// ============================================================
// 3. 构造函数和析构函数
// ============================================================

class FileHandler {
private:
    string m_filename;
    bool m_isOpen;

public:
    // 构造函数：和类名相同，没有返回值
    // 创建对象时自动调用
    FileHandler(const string &filename) {
        m_filename = filename;
        m_isOpen = true;
        cout << "[构造] 打开文件: " << m_filename << endl;
    }

    // 析构函数：~类名，没有参数，没有返回值
    // 对象销毁时自动调用（离开作用域、delete 等）
    ~FileHandler() {
        if (m_isOpen) {
            cout << "[析构] 关闭文件: " << m_filename << endl;
            m_isOpen = false;
        }
    }

    void read() const {
        cout << "[读取] " << m_filename << endl;
    }
};


// ============================================================
// 4. 构造函数的多种写法
// ============================================================

class Camera {
private:
    string m_device;
    int m_width;
    int m_height;
    int m_fps;

public:
    // 写法一：函数体内赋值
    Camera() {
        m_device = "/dev/video0";
        m_width = 640;
        m_height = 480;
        m_fps = 30;
    }

    // 写法二：初始化列表（推荐，效率更高）
    Camera(const string &device, int width, int height)
        : m_device(device), m_width(width), m_height(height), m_fps(30)
    {
        // 初始化列表在进入函数体之前就完成了赋值
        // 对于 string 等复杂类型，初始化列表避免了"先默认构造再赋值"的开销
    }

    // 写法三：带默认参数
    Camera(const string &device, int width, int height, int fps = 30)
        : m_device(device), m_width(width), m_height(height), m_fps(fps)
    {}
    // 注意：写法二和写法三不能同时存在（会有歧义），这里只是展示语法
    // 实际代码中选一种

    void info() const {
        cout << m_device << " " << m_width << "x" << m_height << " @" << m_fps << "fps" << endl;
    }
};


// ============================================================
// 5. 对象的创建方式
// ============================================================

void demo_create_objects() {
    cout << "\n===== 对象创建方式 =====" << endl;

    // 方式一：栈上创建（最常用，离开作用域自动销毁）
    FileHandler f1("config.txt");
    f1.read();

    // 方式二：堆上创建（用 new，需要手动 delete）
    FileHandler *f2 = new FileHandler("data.bin");
    f2->read();        // 指针用 -> 访问成员
    delete f2;         // 必须手动释放，否则内存泄漏！

    // 方式三：数组
    // Point points[3];  // 创建 3 个 Point 对象

    cout << "--- 离开作用域，f1 自动析构 ---" << endl;
}
// f1 在这里自动析构


// ============================================================
// 6. this 指针
// ============================================================

class Counter {
private:
    int m_count;

public:
    Counter() : m_count(0) {}

    // this 是指向当前对象的指针
    // 当参数名和成员名冲突时，用 this-> 区分
    void setCount(int count) {
        this->m_count = count;  // this->m_count 是成员，count 是参数
    }

    // 返回 *this 实现链式调用
    Counter& add(int n) {
        m_count += n;
        return *this;  // 返回自身的引用
    }

    void print() const {
        cout << "count = " << m_count << endl;
    }
};

// ============================================================
// 6.1. 链式调用的解释
// ============================================================
/*
//这是 C++ 类成员函数返回对象自身引用 的标准写法核心就是：*return this;
//1. 逐行翻译
// 函数：加数字，然后返回自己（的引用）
Counter& add(int n) {
    m_count += n;           // 把成员变量 m_count 加上 n
    return *this;           // 🔥 返回当前对象本身（引用）
}
//2. *this 到底是什么？
//this = 指向当前对象的指针
//*this = 指针解引用 → 当前对象本身
//所以：
return *this;
//意思就是：把这个对象自己返回出去！
//3. 为什么返回 Counter&？
//& 代表引用作用：返回对象本身，不复制、不产生新对象，效率高！
//4. 这种写法最大好处：链式调用！
//因为返回了自己，所以可以一直。下去
counter.add(1).add(2).add(3);
//等价于：
counter.add(1);
counter.add(2);
counter.add(3);
//这就是 链式调用（链式编程）Qt、OpenCV、STL 里到处都是
*/

// ============================================================
// 7. const 成员函数
// ============================================================

class Sensor {
private:
    int m_value;

public:
    Sensor(int val) : m_value(val) {}

    // const 成员函数：承诺不修改任何成员变量
    // 如果对象是 const 的，只能调用 const 成员函数
    int getValue() const { return m_value; }

    // 非 const 成员函数：可以修改成员
    void setValue(int val) { m_value = val; }
};

void useSensor(const Sensor &s) {
    // s 是 const 引用，只能调用 const 函数
    cout << "传感器值: " << s.getValue() << endl;
    // s.setValue(100);  // 编译错误！const 对象不能调用非 const 函数
}


int main() {
    // --- 1. C vs C++ ---
    cout << "===== C vs C++ =====" << endl;
    struct Point_C pc = {10, 20};
    point_print_c(&pc);

    Point pp;
    pp.x = 10;
    pp.y = 20;
    pp.print();

    // --- 2. 访问控制 ---
    cout << "\n===== 访问控制 =====" << endl;
    Student stu;
    stu.setName("张三");
    stu.setAge(22);
    stu.setScore(95);
    stu.introduce();

    stu.setAge(-5);  // 会提示"年龄无效"
    // stu.m_name = "xxx";  // 编译错误！m_name 是 private

    // --- 3. 构造和析构 ---
    cout << "\n===== 构造和析构 =====" << endl;
    demo_create_objects();

    // --- 4. this 和链式调用 ---
    cout << "\n===== 链式调用 =====" << endl;
    Counter c;
    c.add(5).add(3).add(2);  // 链式调用
    c.print();  // count = 10

    // --- 5. const ---
    cout << "\n===== const 成员函数 =====" << endl;
    Sensor sensor(42);
    useSensor(sensor);

    return 0;
}

//另一个简单的例子
#include <string>
using namespace std;

class Camera
{
public:
    Camera(const string &device, int width, int height)
        : m_device(device), m_width(width), m_height(height), m_fps(30)
    {
        // this 指针：指向当前正在创建的这个对象
        // 可以用 this-> 访问自己的成员变量
    }

    // 加一个成员函数，演示 this 指针
    void showInfo()
    {
        // this-> 代表“当前这个对象”
        cout << "设备: " << this->m_device << endl;
        cout << "分辨率: " << this->m_width << "x" << this->m_height << endl;
        cout << "帧率: " << this->m_fps << endl;
    }

private:
    string m_device;
    int    m_width;
    int    m_height;
    int    m_fps;
};


int main()
{
    // 创建对象 cam
    Camera cam("/dev/video0", 1280, 720);

    // 调用成员函数，里面会用 this 指针
    cam.showInfo();

    return 0;
}