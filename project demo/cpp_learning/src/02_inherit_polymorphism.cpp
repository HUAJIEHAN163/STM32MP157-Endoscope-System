/**
 * 02_inherit_polymorphism.cpp — 多态（虚函数）
 *
 * 多态是继承的精华：用父类指针调用子类方法
 * Qt 中大量使用：
 *   QThread::run()        → 你覆盖 run() 实现采集逻辑
 *   QWidget::paintEvent() → 你覆盖 paintEvent() 实现自定义绘制
 *   QWidget::keyPressEvent() → 你覆盖它处理键盘事件
 *
 * 编译：g++ 02_inherit_polymorphism.cpp -o 02_poly -std=c++11 && ./02_poly
 */

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. 没有虚函数的问题
// ============================================================

class AnimalNoVirtual {
public:
    void speak() const { cout << "..." << endl; }
};

class DogNoVirtual : public AnimalNoVirtual {
public:
    void speak() const { cout << "汪汪!" << endl; }
};

void demo_no_virtual() {
    cout << "===== 没有虚函数 =====" << endl;
    DogNoVirtual dog;
    dog.speak();  // 输出"汪汪!" — 正常

    // 但是用父类指针指向子类对象时：
    AnimalNoVirtual *p = &dog;
    p->speak();   // 输出"..." — 错了！调用的是父类的版本
    // 因为编译器看到 p 的类型是 AnimalNoVirtual*，就调用 AnimalNoVirtual::speak()
}


// ============================================================
// 2. 虚函数（virtual）解决问题
// ============================================================

class Animal {
public:
    // ⭐ virtual 关键字：告诉编译器"运行时根据实际对象类型决定调用哪个版本"
    virtual void speak() const { cout << "..." << endl; }

    virtual void describe() const {
        cout << "我是一个动物" << endl;
    }

    // ⭐ 虚析构函数：基类必须有！
    // 否则通过父类指针 delete 子类对象时，子类析构函数不会被调用 → 内存泄漏
    virtual ~Animal() {
        cout << "[Animal 析构]" << endl;
    }
};

class Dog : public Animal {
public:
    // override 关键字（C++11）：明确表示覆盖父类虚函数
    // 如果函数签名写错了，编译器会报错（没有 override 的话只是静默创建新函数）
    void speak() const override { cout << "汪汪!" << endl; }
    void describe() const override { cout << "我是一只狗" << endl; }
    ~Dog() override { cout << "[Dog 析构]" << endl; }
};

class Cat : public Animal {
public:
    void speak() const override { cout << "喵~" << endl; }
    void describe() const override { cout << "我是一只猫" << endl; }
    ~Cat() override { cout << "[Cat 析构]" << endl; }
};

void demo_virtual() {
    cout << "\n===== 有虚函数 =====" << endl;
    Dog dog;
    Cat cat;

    // 用父类指针指向不同的子类对象
    Animal *p1 = &dog;
    Animal *p2 = &cat;

    p1->speak();     // 输出"汪汪!" — 正确！运行时调用 Dog::speak()
    p2->speak();     // 输出"喵~"   — 正确！运行时调用 Cat::speak()
    p1->describe();  // 输出"我是一只狗"
    p2->describe();  // 输出"我是一只猫"
}


// ============================================================
// 3. 多态的实际用途：统一接口处理不同对象
// ============================================================

void makeAllSpeak(const vector<Animal*> &animals) {
    // 不需要知道具体是什么动物，统一调用 speak()
    for (const Animal *a : animals) {
        a->speak();  // 运行时自动调用正确的版本
    }
}

void demo_polymorphism_usage() {
    cout << "\n===== 多态的实际用途 =====" << endl;
    Dog dog;
    Cat cat;

    vector<Animal*> zoo = {&dog, &cat, &dog};
    makeAllSpeak(zoo);
    // 输出：汪汪! 喵~ 汪汪!
}


// ============================================================
// 4. 纯虚函数和抽象类
// ============================================================

// 纯虚函数：= 0，没有实现，子类必须覆盖
// 有纯虚函数的类叫抽象类，不能创建对象

// 项目场景：定义图像处理的统一接口

class ImageFilter {
public:
    virtual ~ImageFilter() {}

    // ⭐ 纯虚函数：子类必须实现
    virtual void apply(unsigned char *data, int width, int height) = 0;
    virtual string name() const = 0;
};

// ImageFilter filter;  // 编译错误！抽象类不能创建对象

class SharpenFilter : public ImageFilter {
public:
    void apply(unsigned char *data, int width, int height) override {
        cout << "应用锐化滤镜 " << width << "x" << height << endl;
    }
    string name() const override { return "锐化"; }
};

class DenoiseFilter : public ImageFilter {
public:
    void apply(unsigned char *data, int width, int height) override {
        cout << "应用降噪滤镜 " << width << "x" << height << endl;
    }
    string name() const override { return "降噪"; }
};

class CLAHEFilter : public ImageFilter {
public:
    void apply(unsigned char *data, int width, int height) override {
        cout << "应用 CLAHE 增强 " << width << "x" << height << endl;
    }
    string name() const override { return "CLAHE"; }
};

void demo_abstract_class() {
    cout << "\n===== 纯虚函数和抽象类 =====" << endl;

    // 用基类指针管理不同的滤镜
    vector<ImageFilter*> filters;
    SharpenFilter sharpen;
    DenoiseFilter denoise;
    CLAHEFilter clahe;

    filters.push_back(&sharpen);
    filters.push_back(&denoise);
    filters.push_back(&clahe);

    unsigned char fakeData[100];
    for (ImageFilter *f : filters) {
        cout << "  [" << f->name() << "] ";
        f->apply(fakeData, 640, 480);
    }
}


// ============================================================
// 5. 模拟 Qt 的事件覆盖机制
// ============================================================

// Qt 中 QWidget 定义了很多 virtual 事件处理函数
// 你通过覆盖它们来实现自定义行为

class Widget {
public:
    virtual ~Widget() {}

    // 模拟 Qt 的事件分发
    void processEvents() {
        paintEvent();
        keyPressEvent(65);  // 模拟按下 'A' 键
    }

protected:
    // 这些是 virtual 的，子类可以覆盖
    virtual void paintEvent() {
        cout << "Widget::paintEvent() 默认绘制" << endl;
    }

    virtual void keyPressEvent(int key) {
        cout << "Widget::keyPressEvent() 按键 " << key << endl;
    }
};

// 模拟你的 MainWindow
class MyMainWindow : public Widget {
protected:
    // ⭐ 覆盖 paintEvent，绘制摄像头画面
    void paintEvent() override {
        cout << "MyMainWindow::paintEvent() 绘制摄像头画面" << endl;
    }

    // ⭐ 覆盖 keyPressEvent，处理快捷键
    void keyPressEvent(int key) override {
        if (key == 65) {
            cout << "MyMainWindow::keyPressEvent() 按下A键 → 拍照" << endl;
        } else {
            Widget::keyPressEvent(key);  // 其他键交给父类处理
        }
    }
};

void demo_qt_events() {
    cout << "\n===== 模拟 Qt 事件覆盖 =====" << endl;
    MyMainWindow window;
    window.processEvents();
}


int main() {
    demo_no_virtual();
    demo_virtual();
    demo_polymorphism_usage();
    demo_abstract_class();
    demo_qt_events();

    // 虚析构函数的重要性
    cout << "\n===== 虚析构函数 =====" << endl;
    {
        Animal *p = new Dog();
        delete p;
        // 有虚析构：先 ~Dog() 再 ~Animal() ✅
        // 没有虚析构：只调用 ~Animal()，Dog 的资源泄漏 ❌
    }

    return 0;
}
