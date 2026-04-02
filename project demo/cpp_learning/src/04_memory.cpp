/**
 * 04_memory.cpp — 内存管理
 *
 * C 用 malloc/free，容易忘记释放 → 内存泄漏
 * C++ 用 RAII + 智能指针，自动管理内存
 *
 * 项目中的体现：
 *   OpenCV 的 cv::Mat 自动管理图像内存（引用计数）
 *   Qt 的父子对象树自动释放子对象
 *   V4l2Camera 析构时自动 munmap + close
 *
 * 编译：g++ 04_memory.cpp -o 04_memory -std=c++11 && ./04_memory
 */

#include <iostream>
#include <memory>
#include <string>
using namespace std;

// ============================================================
// 1. C 风格的内存管理问题
// ============================================================

void demo_c_style_problems() {
    cout << "===== C 风格问题 =====" << endl;

    // 问题一：忘记 free → 内存泄漏
    // int *p = (int*)malloc(100 * sizeof(int));
    // ... 用完了忘记 free(p)

    // 问题二：多次 free → 程序崩溃
    // free(p);
    // free(p);  // double free!

    // 问题三：异常导致 free 不执行
    // int *p = new int[100];
    // doSomething();  // 如果这里抛异常，下面的 delete 不会执行
    // delete[] p;     // 泄漏！

    cout << "（这些问题用智能指针解决）" << endl;
}


// ============================================================
// 2. RAII — 资源获取即初始化
// ============================================================

// RAII 的核心思想：
// 构造函数中获取资源（打开文件、分配内存、映射内存）
// 析构函数中释放资源（关闭文件、释放内存、取消映射）
// 对象离开作用域时自动析构 → 资源自动释放

// ⭐ 项目中的 RAII 实例：V4l2Camera
class CameraRAII {
private:
    int m_fd;
    void *m_mmap_ptr;
    size_t m_mmap_size;

public:
    CameraRAII(const string &device) : m_fd(-1), m_mmap_ptr(nullptr), m_mmap_size(0) {
        // 构造时获取资源
        // m_fd = open(device.c_str(), O_RDWR);
        m_fd = 5;  // 模拟
        m_mmap_size = 640 * 480 * 2;
        m_mmap_ptr = new char[m_mmap_size];  // 模拟 mmap
        cout << "RAII: 打开设备 fd=" << m_fd << ", mmap " << m_mmap_size << " 字节" << endl;
    }

    ~CameraRAII() {
        // 析构时释放资源（自动调用，不会忘记）
        if (m_mmap_ptr) {
            delete[] (char*)m_mmap_ptr;  // 模拟 munmap
            cout << "RAII: munmap" << endl;
        }
        if (m_fd >= 0) {
            // close(m_fd);
            cout << "RAII: close fd=" << m_fd << endl;
        }
    }
};


// ============================================================
// 3. unique_ptr — 独占所有权的智能指针
// ============================================================

class Sensor {
public:
    string name;
    Sensor(const string &n) : name(n) { cout << "Sensor 创建: " << name << endl; }
    ~Sensor() { cout << "Sensor 销毁: " << name << endl; }
    void read() const { cout << name << " 读数: 42" << endl; }
};

void demo_unique_ptr() {
    cout << "\n===== unique_ptr =====" << endl;

    // ⭐ unique_ptr：独占所有权，离开作用域自动 delete
    {
        unique_ptr<Sensor> sensor = make_unique<Sensor>("温度传感器");
        sensor->read();  // 用 -> 访问，和普通指针一样
        // 离开作用域，自动 delete，不需要手动释放
    }
    cout << "（已自动释放）" << endl;

    // unique_ptr 不能拷贝（独占！）
    // unique_ptr<Sensor> s1 = make_unique<Sensor>("A");
    // unique_ptr<Sensor> s2 = s1;  // 编译错误！

    // 但可以移动（转移所有权）
    unique_ptr<Sensor> s1 = make_unique<Sensor>("压力传感器");
    unique_ptr<Sensor> s2 = move(s1);  // s1 变为空，s2 拥有对象
    if (!s1) cout << "s1 已为空" << endl;
    s2->read();

    // ⭐ 项目中的用法：
    // unique_ptr<VideoWriter> m_writer;
    // m_writer = make_unique<VideoWriter>("output.avi", ...);
}


// ============================================================
// 4. shared_ptr — 共享所有权的智能指针
// ============================================================

void demo_shared_ptr() {
    cout << "\n===== shared_ptr =====" << endl;

    // shared_ptr：多个指针可以指向同一个对象
    // 内部有引用计数，最后一个 shared_ptr 销毁时才 delete 对象
    // OpenCV 的 cv::Mat 就是类似的机制

    shared_ptr<Sensor> s1 = make_shared<Sensor>("光传感器");
    cout << "引用计数: " << s1.use_count() << endl;  // 1

    {
        shared_ptr<Sensor> s2 = s1;  // 共享所有权
        cout << "引用计数: " << s1.use_count() << endl;  // 2
        s2->read();
    }
    // s2 离开作用域，引用计数减为 1，对象还在

    cout << "引用计数: " << s1.use_count() << endl;  // 1
    s1->read();  // 对象还活着

    // s1 离开作用域，引用计数减为 0，对象被销毁
}


// ============================================================
// 5. new/delete vs 智能指针 对比
// ============================================================

void demo_comparison() {
    cout << "\n===== 对比 =====" << endl;

    // ❌ 裸指针：容易泄漏
    // Sensor *p = new Sensor("A");
    // ... 如果中间 return 或抛异常，下面的 delete 不执行
    // delete p;

    // ✅ unique_ptr：自动释放，零开销
    auto p = make_unique<Sensor>("B");
    // 不需要 delete，离开作用域自动释放

    // 选择指南：
    // 1. 优先用栈对象（不用 new）
    //    Sensor s("C");  // 最简单，自动释放
    //
    // 2. 需要堆分配时用 unique_ptr
    //    auto p = make_unique<Sensor>("D");
    //
    // 3. 需要共享所有权时用 shared_ptr
    //    auto p = make_shared<Sensor>("E");
    //
    // 4. 尽量不用裸 new/delete
}


// ============================================================
// 6. Qt 的内存管理：父子对象树
// ============================================================

// Qt 有自己的内存管理机制，不完全依赖智能指针
// 核心规则：指定了 parent 的对象，parent 析构时自动 delete 子对象

// 模拟 Qt 的父子关系
class QObjectSimple {
private:
    string m_name;
    QObjectSimple *m_parent;
    vector<QObjectSimple*> m_children;

public:
    QObjectSimple(const string &name, QObjectSimple *parent = nullptr)
        : m_name(name), m_parent(parent)
    {
        if (m_parent) {
            m_parent->m_children.push_back(this);
        }
        cout << "创建: " << m_name << endl;
    }

    ~QObjectSimple() {
        // ⭐ 析构时自动删除所有子对象
        for (auto child : m_children) {
            delete child;
        }
        cout << "销毁: " << m_name << endl;
    }
};

void demo_qt_memory() {
    cout << "\n===== Qt 父子对象树 =====" << endl;

    // 项目中的写法：
    // QPushButton *btn = new QPushButton("拍照", this);
    //                                           ↑ this 是 parent
    // 不需要手动 delete btn，MainWindow 析构时自动删除

    QObjectSimple *window = new QObjectSimple("MainWindow");
    new QObjectSimple("Button_拍照", window);    // parent = window
    new QObjectSimple("Button_录像", window);    // parent = window
    new QObjectSimple("Label_FPS", window);      // parent = window

    cout << "--- 删除 window ---" << endl;
    delete window;  // 自动删除所有子对象
}


int main() {
    demo_c_style_problems();

    cout << "\n===== RAII =====" << endl;
    {
        CameraRAII cam("/dev/video0");
        // 使用 cam...
    }
    // 离开作用域，自动释放

    demo_unique_ptr();
    demo_shared_ptr();
    demo_comparison();
    demo_qt_memory();

    return 0;
}
