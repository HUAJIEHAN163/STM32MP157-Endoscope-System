/**
 * 02_inherit_basics.cpp — 继承基础
 *
 * 继承是 Qt 的核心机制：
 *   QWidget → QMainWindow → MainWindow（你的主窗口）
 *   QThread → V4l2Camera（你的采集线程）
 *   QFrame  → OneVideo（视频显示窗口）
 *
 * 重点：为什么要继承、怎么继承、构造/析构顺序
 *
 * 编译：g++ 02_inherit_basics.cpp -o 02_inherit_basics -std=c++11 && ./02_inherit_basics
 */

#include <iostream>
#include <string>
using namespace std;

// ============================================================
// 1. 为什么需要继承
// ============================================================

// 不用继承：每个类都要重复写相同的代码
/*
class Dog {
    string name; int age;
    void eat() { ... }
    void sleep() { ... }
    void bark() { ... }   // 狗特有
};
class Cat {
    string name; int age;  // 重复！
    void eat() { ... }     // 重复！
    void sleep() { ... }   // 重复！
    void meow() { ... }   // 猫特有
};
*/

// 用继承：把共同部分提取到基类
class Animal {
protected:  // protected: 子类可以访问，外部不能访问
    string m_name;
    int m_age;

public:
    Animal(const string &name, int age) : m_name(name), m_age(age) {
        cout << "[Animal 构造] " << m_name << endl;
    }

    ~Animal() {
        cout << "[Animal 析构] " << m_name << endl;
    }

    void eat() const { cout << m_name << " 在吃东西" << endl; }
    void sleep() const { cout << m_name << " 在睡觉" << endl; }

    string getName() const { return m_name; }
    int getAge() const { return m_age; }
};

// Dog 继承 Animal，自动拥有 name, age, eat(), sleep()
class Dog : public Animal {
public:
    // 子类构造函数必须调用父类构造函数
    Dog(const string &name, int age) : Animal(name, age) {
        cout << "[Dog 构造] " << m_name << endl;
    }

    ~Dog() {
        cout << "[Dog 析构] " << m_name << endl;
    }

    // Dog 特有的方法
    void bark() const { cout << m_name << ": 汪汪汪!" << endl; }
};

class Cat : public Animal {
public:
    Cat(const string &name, int age) : Animal(name, age) {
        cout << "[Cat 构造] " << m_name << endl;
    }

    ~Cat() {
        cout << "[Cat 析构] " << m_name << endl;
    }

    void meow() const { cout << m_name << ": 喵~" << endl; }
};


// ============================================================
// 2. 三种访问控制在继承中的表现
// ============================================================

class Base {
public:
    int pub;        // 所有人都能访问
protected:
    int prot;       // 自己和子类能访问
private:
    int priv;       // 只有自己能访问
};

class Derived : public Base {
    // public 继承（最常用，Qt 中全部用这种）：
    //   Base 的 public    → Derived 的 public
    //   Base 的 protected → Derived 的 protected
    //   Base 的 private   → 不可访问

public:
    void test() {
        pub = 1;    // OK：public 成员
        prot = 2;   // OK：protected 成员，子类可以访问
        // priv = 3; // 错误！private 成员，子类也不能访问
    }
};

// 记忆口诀：
// public:    谁都能用
// protected: 自家人（子类）能用
// private:   只有自己能用


// ============================================================
// 3. 构造和析构的顺序
// ============================================================

// 构造顺序：先父后子（先打地基再盖楼）
// 析构顺序：先子后父（先拆楼再拆地基）

// 项目中的体现：
// MainWindow 继承 QMainWindow
// 构造时：先 QMainWindow() → 再 MainWindow()
// 析构时：先 ~MainWindow() → 再 ~QMainWindow()


// ============================================================
// 4. 方法覆盖（Override）
// ============================================================

class Shape {
protected:
    string m_color;

public:
    Shape(const string &color) : m_color(color) {}

    void describe() const {
        cout << "这是一个" << m_color << "的图形" << endl;
    }

    double area() const {
        return 0;  // 基类不知道具体面积
    }
};

class Circle : public Shape {
private:
    double m_radius;

public:
    Circle(const string &color, double radius)
        : Shape(color), m_radius(radius) {}

    // 覆盖父类的方法（同名同参数）
    void describe() const {
        cout << "这是一个" << m_color << "的圆，半径=" << m_radius << endl;
    }

    double area() const {
        return 3.14159 * m_radius * m_radius;
    }
};

class Rectangle : public Shape {
private:
    double m_width, m_height;

public:
    Rectangle(const string &color, double w, double h)
        : Shape(color), m_width(w), m_height(h) {}

    void describe() const {
        cout << "这是一个" << m_color << "的矩形，"
             << m_width << "x" << m_height << endl;
    }

    double area() const {
        return m_width * m_height;
    }
};


// ============================================================
// 5. 项目中的继承实例
// ============================================================

// 模拟 Qt 的 QThread 继承模式
// 这就是 V4l2Camera 的设计原型

class SimpleThread {
public:
    SimpleThread() { cout << "[SimpleThread 构造]" << endl; }
    ~SimpleThread() { cout << "[SimpleThread 析构]" << endl; }

    void start() {
        cout << "线程启动..." << endl;
        run();  // 调用子类实现的 run
    }

    // 子类需要覆盖这个方法
    void run() {
        cout << "SimpleThread::run() 默认实现（什么都不做）" << endl;
    }
};

// 模拟 V4l2Camera 继承 QThread
class MyCameraThread : public SimpleThread {
private:
    string m_device;

public:
    MyCameraThread(const string &device)
        : SimpleThread(), m_device(device)
    {
        cout << "[MyCameraThread 构造] " << m_device << endl;
    }

    ~MyCameraThread() {
        cout << "[MyCameraThread 析构]" << endl;
    }

    // 覆盖 run()，实现自己的采集逻辑
    void run() {
        cout << "MyCameraThread::run() 从 " << m_device << " 采集图像" << endl;
    }
};


int main() {
    // --- 1. 基本继承 ---
    cout << "===== 基本继承 =====" << endl;
    Dog dog("旺财", 3);
    dog.eat();      // 继承自 Animal
    dog.sleep();    // 继承自 Animal
    dog.bark();     // Dog 自己的
    cout << dog.getName() << " " << dog.getAge() << "岁" << endl;

    cout << endl;
    Cat cat("咪咪", 2);
    cat.eat();
    cat.meow();

    // --- 2. 构造/析构顺序 ---
    cout << "\n===== 构造/析构顺序 =====" << endl;
    {
        cout << "--- 创建 Dog ---" << endl;
        Dog d("小黑", 1);
        cout << "--- 离开作用域 ---" << endl;
    }
    // 输出顺序：Animal构造 → Dog构造 → Dog析构 → Animal析构

    // --- 3. 方法覆盖 ---
    cout << "\n===== 方法覆盖 =====" << endl;
    Circle c("红色", 5.0);
    c.describe();
    cout << "面积: " << c.area() << endl;

    Rectangle r("蓝色", 4.0, 6.0);
    r.describe();
    cout << "面积: " << r.area() << endl;

    // --- 4. 项目中的继承 ---
    cout << "\n===== 模拟 QThread 继承 =====" << endl;
    {
        MyCameraThread cam("/dev/video0");
        cam.start();  // 调用 run()，执行的是 MyCameraThread 的版本
    }

    return 0;
}
